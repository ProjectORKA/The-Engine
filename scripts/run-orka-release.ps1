param(
    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

$ErrorActionPreference = "Stop"

$runScript = Join-Path $PSScriptRoot "run-orka.ps1"
if (-not (Test-Path $runScript)) {
    throw "Base run script not found at '$runScript'."
}

if ($Arguments) {
    & $runScript -Configuration Release -Platform $Platform @Arguments
} else {
    & $runScript -Configuration Release -Platform $Platform
}

exit $LASTEXITCODE
