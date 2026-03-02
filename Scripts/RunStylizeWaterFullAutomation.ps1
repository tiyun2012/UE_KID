param(
    [string]$UECmdPath = "G:\TA_Private\Ure\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe",
    [string]$ProjectPath = (Join-Path (Split-Path $PSScriptRoot -Parent) "KIDs.uproject"),
    [string]$ReportOutputPath = (Join-Path (Split-Path $PSScriptRoot -Parent) "Saved\Automation\Stylize"),
    [switch]$SkipBuild,
    [switch]$OpenEditor
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path $ProjectPath -Parent

function Write-Step([string]$Message) {
    Write-Host ""
    Write-Host "==> $Message"
}

function Invoke-UeCmd([string[]]$Arguments, [string]$StepName) {
    Write-Step $StepName
    & $UECmdPath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Step failed with exit code ${LASTEXITCODE}: $StepName"
    }
}

function Invoke-UbtBuild() {
    $EngineDir = Split-Path (Split-Path (Split-Path $UECmdPath -Parent) -Parent) -Parent
    $UbtPath = Join-Path $EngineDir "Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    if (-not (Test-Path $UbtPath)) {
        throw "UnrealBuildTool was not found: $UbtPath"
    }

    $TargetName = ("{0}Editor" -f [System.IO.Path]::GetFileNameWithoutExtension($ProjectPath))
    $buildArgs = @(
        $TargetName,
        "Win64",
        "Development",
        "-Project=$ProjectPath",
        "-WaitMutex",
        "-NoHotReloadFromIDE"
    )

    Write-Step "Build editor target ($TargetName)"
    & $UbtPath @buildArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code ${LASTEXITCODE}."
    }
}

function Invoke-UePythonScript([string]$ScriptName, [string]$StepName) {
    $ScriptPath = Join-Path $ProjectRoot ("Scripts\" + $ScriptName)
    if (-not (Test-Path $ScriptPath)) {
        throw "Missing script: $ScriptPath"
    }

    $args = @(
        $ProjectPath,
        "-run=pythonscript",
        "-script=$ScriptPath",
        "-unattended",
        "-nop4",
        "-nosplash",
        "-nullrhi"
    )

    Invoke-UeCmd -Arguments $args -StepName $StepName
}

function Test-ReportForFailures([string]$ReportPath) {
    $IndexPath = Join-Path $ReportPath "index.json"
    if (-not (Test-Path $IndexPath)) {
        Write-Warning "Automation report was not found at $IndexPath."
        return
    }

    $jsonRaw = Get-Content -Path $IndexPath -Raw
    $json = $jsonRaw | ConvertFrom-Json
    $propertyNames = $json.PSObject.Properties.Name

    $failCount = $null
    foreach ($key in @("failed", "numFailed", "Failed", "tests_failed")) {
        if ($propertyNames -contains $key) {
            $failCount = [int]$json.$key
            break
        }
    }

    if ($null -eq $failCount) {
        Write-Warning "Could not find a failure-count field in $IndexPath. Please inspect report manually."
        return
    }

    if ($failCount -gt 0) {
        throw "Automation tests reported $failCount failure(s). See report: $ReportPath"
    }

    Write-Host "Automation tests passed (failed=$failCount)."
}

if (-not (Test-Path $UECmdPath)) {
    throw "UnrealEditor-Cmd was not found: $UECmdPath"
}

if (-not (Test-Path $ProjectPath)) {
    throw "Project was not found: $ProjectPath"
}

New-Item -Path $ReportOutputPath -ItemType Directory -Force | Out-Null

if (-not $SkipBuild.IsPresent) {
    Invoke-UbtBuild
}

Invoke-UePythonScript -ScriptName "CreateStylizeWaterAssets.py" -StepName "Create/Update stylize water material assets"
Invoke-UePythonScript -ScriptName "VerifyStylizeWaterAssets.py" -StepName "Verify stylize water material assets"
Invoke-UePythonScript -ScriptName "CreateStylizeTestLevel.py" -StepName "Create/Update stylize test level"
Invoke-UePythonScript -ScriptName "VerifyStylizeTestLevel.py" -StepName "Verify stylize test level content"

$automationArgs = @(
    $ProjectPath,
    "-unattended",
    "-nop4",
    "-nosplash",
    "-nullrhi",
    "-ExecCmds=Automation RunTests KIDs.Stylize",
    "-TestExit=Automation Test Queue Empty",
    "-ReportOutputPath=$ReportOutputPath"
)
Invoke-UeCmd -Arguments $automationArgs -StepName "Run automation tests (KIDs.Stylize)"
Test-ReportForFailures -ReportPath $ReportOutputPath

Write-Host ""
Write-Host "Stylize water full automation finished successfully."
Write-Host "Report path: $ReportOutputPath"

if ($OpenEditor.IsPresent) {
    $EditorPath = Join-Path (Split-Path $UECmdPath -Parent) "UnrealEditor.exe"
    if (Test-Path $EditorPath) {
        Write-Step "Opening editor on /Game/Tests/L_StylizeAutoTest"
        Start-Process -FilePath $EditorPath -ArgumentList @($ProjectPath, "/Game/Tests/L_StylizeAutoTest")
    } else {
        Write-Warning "Could not find UnrealEditor.exe at $EditorPath"
    }
}
