<#
.SYNOPSIS
Calls the Project ORKA localhost agent bridge.

.EXAMPLE
.\scripts\orka-agent.ps1 -Token $env:ORKA_AGENT_TOKEN

.EXAMPLE
.\scripts\orka-agent.ps1 -Token secret -Method ui.inspect -Params '{"windowId":1}' -Pretty

.EXAMPLE
.\scripts\orka-agent.ps1 -Token secret -Screenshot -WindowId 1 -OutputPath .\artifacts\frame.png -Pretty
#>
[CmdletBinding()]
param(
    [int] $Port = 42123,
    [string] $Token = $env:ORKA_AGENT_TOKEN,
    [string] $Method = 'agent.health',
    [object] $Params,
    [ValidateRange(1, 2147483647)]
    [int] $TimeoutMs = 5000,
    [switch] $Pretty,

    [switch] $ListMethods,
    [switch] $Windows,
    [switch] $Screenshot,
    [switch] $MouseMove,
    [switch] $Click,
    [string] $KeyPress,
    [string] $Text,
    [switch] $Logs,
    [switch] $ProfileStart,
    [switch] $ProfileStop,
    [switch] $ProfileRead,
    [ValidateRange(1, 2147483647)]
    [int] $WaitFrames,

    [long] $WindowId,
    [string] $OutputPath,
    [double] $X,
    [double] $Y,
    [ValidateSet('left', 'middle', 'right', 'x1', 'x2')]
    [string] $Button = 'left',
    [ValidateRange(1, 100)]
    [int] $ClickCount = 1,
    [string[]] $Modifiers,
    [long] $AfterId,
    [ValidateRange(1, 100000)]
    [int] $Limit = 1000,
    [string] $ProfileId,
    [string] $ProfileName
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$script:OrkaBoundParameters = $PSBoundParameters

function Test-Bound {
    param([string] $Name)
    return $script:OrkaBoundParameters.ContainsKey($Name)
}

function ConvertTo-ParameterObject {
    param([object] $Value)

    if ($null -eq $Value) {
        return [pscustomobject]@{}
    }

    if ($Value -is [string]) {
        $json = $Value.Trim()
        if ([string]::IsNullOrWhiteSpace($json)) {
            return [pscustomobject]@{}
        }

        try {
            return ($json | ConvertFrom-Json)
        }
        catch {
            throw "Invalid -Params JSON: $($_.Exception.Message)"
        }
    }

    return $Value
}

function Read-ExactBytes {
    param(
        [System.IO.Stream] $Stream,
        [int] $Count
    )

    $buffer = New-Object byte[] $Count
    $offset = 0
    while ($offset -lt $Count) {
        $read = $Stream.Read($buffer, $offset, $Count - $offset)
        if ($read -le 0) {
            throw "The agent bridge closed the connection after $offset of $Count expected bytes."
        }
        $offset += $read
    }
    return $buffer
}

function Add-IfBound {
    param(
        [hashtable] $Target,
        [string] $ParameterName,
        [string] $PropertyName = $ParameterName
    )

    if (Test-Bound $ParameterName) {
        $Target[$PropertyName] = Get-Variable -Name $ParameterName -Scope Script -ValueOnly
    }
}

if ([string]::IsNullOrWhiteSpace($Token)) {
    throw 'An agent token is required. Pass -Token or set the ORKA_AGENT_TOKEN environment variable.'
}
if ([string]::IsNullOrWhiteSpace($Method)) {
    throw '-Method cannot be empty.'
}
if ($Port -lt 1 -or $Port -gt 65535) {
    throw '-Port must be between 1 and 65535.'
}

$convenienceCount = 0
@(
    $ListMethods.IsPresent,
    $Windows.IsPresent,
    $Screenshot.IsPresent,
    $MouseMove.IsPresent,
    $Click.IsPresent,
    (Test-Bound 'KeyPress'),
    (Test-Bound 'Text'),
    $Logs.IsPresent,
    $ProfileStart.IsPresent,
    $ProfileStop.IsPresent,
    $ProfileRead.IsPresent,
    (Test-Bound 'WaitFrames')
) | ForEach-Object {
    if ($_ -eq $true) {
        $convenienceCount++
    }
}
if ($convenienceCount -gt 1) {
    throw 'Specify only one convenience operation at a time.'
}

$requestMethod = $Method
$requestParams = ConvertTo-ParameterObject $Params

if ($convenienceCount -eq 1) {
    $values = @{}
    if ($ListMethods) {
        $requestMethod = 'agent.listMethods'
    }
    elseif ($Windows) {
        $requestMethod = 'window.list'
    }
    elseif ($Screenshot) {
        $requestMethod = 'screenshot.capture'
        Add-IfBound $values 'WindowId' 'windowId'
        Add-IfBound $values 'OutputPath' 'path'
    }
    elseif ($MouseMove) {
        if (-not (Test-Bound 'X') -or -not (Test-Bound 'Y')) {
            throw '-MouseMove requires -X and -Y.'
        }
        $requestMethod = 'input.pointerMove'
        $values.x = $X
        $values.y = $Y
        Add-IfBound $values 'WindowId' 'windowId'
    }
    elseif ($Click) {
        $requestMethod = 'input.click'
        Add-IfBound $values 'X' 'x'
        Add-IfBound $values 'Y' 'y'
        if ((Test-Bound 'X') -xor (Test-Bound 'Y')) {
            throw 'When clicking at a position, provide both -X and -Y.'
        }
        $values.button = $Button
        $values.clickCount = $ClickCount
        Add-IfBound $values 'WindowId' 'windowId'
    }
    elseif (Test-Bound 'KeyPress') {
        if ([string]::IsNullOrWhiteSpace($KeyPress)) {
            throw '-KeyPress requires a non-empty key name.'
        }
        $requestMethod = 'input.key'
        $values.key = $KeyPress
        if ($null -ne $Modifiers -and $Modifiers.Count -gt 0) {
            $values.modifiers = $Modifiers
        }
        Add-IfBound $values 'WindowId' 'windowId'
    }
    elseif (Test-Bound 'Text') {
        $requestMethod = 'input.text'
        $values.text = $Text
        Add-IfBound $values 'WindowId' 'windowId'
    }
    elseif ($Logs) {
        $requestMethod = 'logs.read'
        Add-IfBound $values 'AfterId' 'afterId'
        $values.limit = $Limit
    }
    elseif ($ProfileStart) {
        $requestMethod = 'profile.start'
        Add-IfBound $values 'ProfileName' 'name'
    }
    elseif ($ProfileStop) {
        $requestMethod = 'profile.stop'
        Add-IfBound $values 'ProfileId' 'profileId'
    }
    elseif ($ProfileRead) {
        $requestMethod = 'profile.read'
        Add-IfBound $values 'ProfileId' 'profileId'
        Add-IfBound $values 'OutputPath' 'path'
        Add-IfBound $values 'WindowId' 'windowId'
    }
    elseif (Test-Bound 'WaitFrames') {
        $requestMethod = 'agent.waitFrames'
        $values.frames = $WaitFrames
        Add-IfBound $values 'WindowId' 'windowId'
    }
    $requestParams = [pscustomobject]$values
}

$requestId = [guid]::NewGuid().ToString('N')
$request = [ordered]@{
    version = 1
    id = $requestId
    token = $Token
    method = $requestMethod
    params = $requestParams
    timeoutMs = $TimeoutMs
}

$utf8 = New-Object System.Text.UTF8Encoding($false)
$payload = $utf8.GetBytes(($request | ConvertTo-Json -Depth 32 -Compress))
$lengthBytes = [BitConverter]::GetBytes([int]$payload.Length)
if (-not [BitConverter]::IsLittleEndian) {
    [Array]::Reverse($lengthBytes)
}

$client = New-Object System.Net.Sockets.TcpClient
try {
    $connect = $client.BeginConnect('127.0.0.1', $Port, $null, $null)
    try {
        if (-not $connect.AsyncWaitHandle.WaitOne($TimeoutMs, $false)) {
            throw "Timed out after $TimeoutMs ms connecting to the ORKA agent bridge on 127.0.0.1:$Port."
        }
        $client.EndConnect($connect)
    }
    finally {
        $connect.AsyncWaitHandle.Close()
    }

    $client.ReceiveTimeout = $TimeoutMs
    $client.SendTimeout = $TimeoutMs
    $stream = $client.GetStream()
    $stream.ReadTimeout = $TimeoutMs
    $stream.WriteTimeout = $TimeoutMs

    $stream.Write($lengthBytes, 0, $lengthBytes.Length)
    $stream.Write($payload, 0, $payload.Length)
    $stream.Flush()

    $responseLengthBytes = Read-ExactBytes $stream 4
    if (-not [BitConverter]::IsLittleEndian) {
        [Array]::Reverse($responseLengthBytes)
    }
    $responseLength = [BitConverter]::ToInt32($responseLengthBytes, 0)
    if ($responseLength -le 0 -or $responseLength -gt 67108864) {
        throw "The agent bridge returned an invalid frame length: $responseLength."
    }

    $responseJson = $utf8.GetString((Read-ExactBytes $stream $responseLength))
    try {
        $response = $responseJson | ConvertFrom-Json
    }
    catch {
        throw "The agent bridge returned invalid JSON: $($_.Exception.Message)"
    }

    if ($null -ne $response.PSObject.Properties['id'] -and [string]$response.id -ne $requestId) {
        throw "Agent response id '$($response.id)' does not match request id '$requestId'."
    }

    if ($null -ne $response.PSObject.Properties['error'] -and $null -ne $response.error) {
        $code = if ($null -ne $response.error.PSObject.Properties['code']) { [string]$response.error.code } else { 'agent_error' }
        $message = if ($null -ne $response.error.PSObject.Properties['message']) { [string]$response.error.message } else { 'The agent request failed.' }
        $details = if ($null -ne $response.error.PSObject.Properties['data']) {
            ' Details: ' + ($response.error.data | ConvertTo-Json -Depth 16 -Compress)
        } else {
            ''
        }
        throw "ORKA agent error [$code] for '$requestMethod': $message$details"
    }

    $output = if ($null -ne $response.PSObject.Properties['result']) { $response.result } else { $response }
    if ($Pretty) {
        $output | ConvertTo-Json -Depth 32
    }
    else {
        Write-Output $output
    }
}
catch [System.Net.Sockets.SocketException] {
    throw "Could not communicate with the ORKA agent bridge on 127.0.0.1:${Port}: $($_.Exception.Message)"
}
catch [System.IO.IOException] {
    throw "ORKA agent bridge I/O failed or timed out after $TimeoutMs ms: $($_.Exception.Message)"
}
finally {
    $client.Close()
}
