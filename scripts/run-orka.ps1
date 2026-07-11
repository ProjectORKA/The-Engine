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

    $existing = @()
    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            $existing += Get-Item $candidate
        }
    }

    if ($existing.Count -gt 0) {
        return ($existing | Sort-Object LastWriteTime -Descending | Select-Object -First 1).FullName
    }

    return $candidates[0]
}

function Sync-OrkaRuntimeData {
    param(
        [string]$ExePath,
        [string]$RepoRoot
    )

    $exeDir = Split-Path -Parent $ExePath
    $sourceData = Join-Path $RepoRoot "Project ORKA\Data"
    $targetData = Join-Path $exeDir "Data"

    if (-not (Test-Path $sourceData)) {
        return
    }

    if (-not (Test-Path $targetData)) {
        Write-Host "Copying runtime Data folder to '$targetData'..."
        New-Item -ItemType Directory -Path $targetData -Force | Out-Null
    }

    & robocopy $sourceData $targetData /E /NFL /NDL /NJH /NJS /NC /NS /NP | Out-Null
    if ($LASTEXITCODE -ge 8) {
        throw "Failed to copy Data folder to '$targetData'."
    }
}

Write-Host "Building before launch..."
& (Join-Path $PSScriptRoot "build-orka.ps1") -Configuration $Configuration -Platform $Platform

$exePath = Get-OrkaExecutablePath -Root $repoRoot -ProjectRoot $projectDir -Config $Configuration -Plat $Platform
if (-not (Test-Path $exePath)) {
    throw "Executable not found after build. Expected near '$exePath'."
}

Sync-OrkaRuntimeData -ExePath $exePath -RepoRoot $repoRoot

$exeDir = Split-Path -Parent $exePath

Write-Host "Launching: $exePath"
Write-Host "Working directory: $exeDir"
if ($Arguments) {
    Write-Host "Arguments: $($Arguments -join ' ')"
}

Push-Location $exeDir
try {
    if ($Arguments) {
        & "$exePath" @Arguments
    } else {
        & "$exePath"
    }
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
