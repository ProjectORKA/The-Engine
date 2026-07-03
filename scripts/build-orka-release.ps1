param(
    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",

    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$buildScript = Join-Path $PSScriptRoot "build-orka.ps1"
if (-not (Test-Path $buildScript)) {
    throw "Base build script not found at '$buildScript'."
}

& $buildScript -Configuration Release -Platform $Platform -Clean:$Clean
exit $LASTEXITCODE
