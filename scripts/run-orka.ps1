param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",

    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$projectDir = Join-Path $repoRoot "Project ORKA"
$exeName = "Project ORKA.exe"

function Get-OrkaExecutablePath {
    param(
        [string]$Root,
        [string]$ProjectRoot,
        [string]$Config,
        [string]$Plat
    )

    $candidates = if ($Plat -eq "x64") {
        @(
            (Join-Path $Root "$Plat\$Config\$exeName"),
            (Join-Path $ProjectRoot "$Plat\$Config\$exeName")
        )
    } else {
        @(
            (Join-Path $Root "$Config\$exeName"),
            (Join-Path $ProjectRoot "$Config\$exeName")
        )
    }

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return $candidate
        }
    }

    return $candidates[0]
}

Write-Host "Building before launch..."
& (Join-Path $PSScriptRoot "build-orka.ps1") -Configuration $Configuration -Platform $Platform

$exePath = Get-OrkaExecutablePath -Root $repoRoot -ProjectRoot $projectDir -Config $Configuration -Plat $Platform
if (-not (Test-Path $exePath)) {
    throw "Executable not found after build. Expected near '$exePath'."
}

$exeDir = Split-Path -Parent $exePath

Write-Host "Launching: $exePath"
Write-Host "Working directory: $exeDir"
if ($Arguments) {
    Write-Host "Arguments: $($Arguments -join ' ')"
}

Push-Location $exeDir
try {
    if ($Arguments) {
        & $exePath @Arguments
    } else {
        & $exePath
    }
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
