param(
    [Parameter(Mandatory = $true)]
    [string]$TargetDir,

    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"

$TargetDir = (Resolve-Path -LiteralPath $TargetDir).Path

$repoRoot = Split-Path -Parent $PSScriptRoot
$dataSource = Join-Path $repoRoot "Project ORKA\Data"
$dataTarget = Join-Path $TargetDir "Data"

if (-not (Test-Path $dataSource)) {
    throw "Data source not found at '$dataSource'."
}

New-Item -ItemType Directory -Force -Path $dataTarget | Out-Null

# Robocopy exit codes 0-7 indicate success; /XO skips files that are already up to date.
robocopy $dataSource $dataTarget /E /XO /NFL /NDL /NJH /NJS /NC /NS | Out-Null
$robocopyExit = $LASTEXITCODE
if ($robocopyExit -ge 8) {
    throw "Robocopy failed while syncing Data with exit code $robocopyExit."
}

function Copy-DllIfNewer {
    param(
        [string]$SourcePattern,
        [string]$DestinationDirectory
    )

    $files = Get-ChildItem -Path $SourcePattern -ErrorAction SilentlyContinue
    foreach ($file in $files) {
        $destination = Join-Path $DestinationDirectory $file.Name
        if (-not (Test-Path $destination) -or $file.LastWriteTimeUtc -gt (Get-Item $destination).LastWriteTimeUtc) {
            Copy-Item -Path $file.FullName -Destination $destination -Force
        }
    }
}

if ($Configuration -eq "Debug") {
    $textDepsBin = Join-Path $repoRoot "text-deps\x64-windows\debug\bin"
    Copy-DllIfNewer (Join-Path $textDepsBin "harfbuzz*.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "icu*.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "msdfgen*.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "tinyxml2.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "zd.dll") $TargetDir
}
else {
    $textDepsBin = Join-Path $repoRoot "text-deps\x64-windows\bin"
    Copy-DllIfNewer (Join-Path $textDepsBin "harfbuzz*.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "icu*.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "msdfgen*.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "tinyxml2.dll") $TargetDir
    Copy-DllIfNewer (Join-Path $textDepsBin "z.dll") $TargetDir

    $vcpkgBin = "C:\vcpkg\installed\x64-windows\bin"
    if (Test-Path $vcpkgBin) {
        Copy-DllIfNewer (Join-Path $vcpkgBin "*.dll") $TargetDir
    }
}
