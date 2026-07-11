param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",

    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",

    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$solutionPath = Join-Path $repoRoot "Project ORKA.sln"

if (-not (Test-Path $solutionPath)) {
    throw "Solution not found at '$solutionPath'."
}

function Get-MSBuildPath {
    $candidates = @(
        "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
    )

    foreach ($path in $candidates) {
        if (Test-Path $path) {
            return $path
        }
    }

    $vsWherePath = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWherePath) {
        $installPath = & $vsWherePath -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
        if ($installPath) {
            $detectedPath = Join-Path $installPath "MSBuild\Current\Bin\MSBuild.exe"
            if (Test-Path $detectedPath) {
                return $detectedPath
            }
        }
    }

    return $null
}

$msbuildPath = Get-MSBuildPath
if (-not $msbuildPath) {
    throw "MSBuild not found. Install Visual Studio (or Build Tools) with C++ workload."
}

$targets = if ($Clean) { "Clean;Build" } else { "Build" }
$arguments = @(
    "`"$solutionPath`"",
    "/restore",
    "/t:$targets",
    "/p:Configuration=$Configuration",
    "/p:Platform=$Platform",
    "/m",
    "/nologo"
)

Write-Host "Using MSBuild: $msbuildPath"
Write-Host "Building: $solutionPath ($Configuration|$Platform)"

& $msbuildPath @arguments
$exitCode = $LASTEXITCODE

if ($exitCode -ne 0) {
    throw "Build failed with exit code $exitCode."
}

Write-Host "Build completed successfully."
