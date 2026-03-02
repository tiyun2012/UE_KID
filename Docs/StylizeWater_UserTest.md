# Stylize Water: User Test Guide

## 1. What you should have now

- Water simulation actor: `AStylizeWaterActor` (`StylizeRender` plugin)
- Auto test map: `/Game/Tests/L_StylizeAutoTest`
- Level generation script: `Scripts/CreateStylizeTestLevel.py`
- Actor-level visual controls: `WaveAmplitude` (default `24.0`) and `FoamIntensity` (default `2.0`)

The water actor now tries to auto-load default templates at construction:

- `/Game/StylizeRender/Materials/M_WaterSim_Template.M_WaterSim_Template`
- `/Game/StylizeRender/Materials/M_WaterSurface_Template.M_WaterSurface_Template`

If these assets are missing, the actor logs warnings in `BeginPlay`.

## 2. Open the test level

### Option A: In editor console

```text
Open /Game/Tests/L_StylizeAutoTest
```

### Option B: Command line

```powershell
& "G:\TA_Private\Ure\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" `
  "D:\dev\INTERNAL\KIDs 5.7\KIDs.uproject" `
  "/Game/Tests/L_StylizeAutoTest"
```

## 3. Create required materials (first-time setup)

Create these assets:

- `M_WaterSim_Template` at `/Game/StylizeRender/Materials/`
- `M_WaterSurface_Template` at `/Game/StylizeRender/Materials/`

### `M_WaterSim_Template` (simulation pass)

- Material Domain: `Surface`
- Shading Model: `Unlit`
- Usage: used by `DrawMaterialToRenderTarget`

Required parameters (exact names):

- Texture parameter: `CurrentState`
- Scalar parameters: `Dt`, `Viscosity`
- Vector parameters: `BoatData`, `Obstacle0`, `Obstacle1`, `Obstacle2`

Output:

- Write simulation state into `Emissive Color` as `(NewHeight, PrevHeight, Foam, 1)`

### `M_WaterSurface_Template` (visible water)

- Shading Model: `SingleLayerWater` (or your preferred stylized setup)
- Texture parameter: `SimulationTexture`

Use channels:

- `R`: wave displacement (to `World Position Offset` Z)
- `B`: foam mask (color/emissive/opacity logic)

## 4. Drive the simulation (boat + obstacles)

`AStylizeWaterActor` needs interaction input every frame to show dynamic wakes.

In Level Blueprint (quick test):

1. `Event Tick`
2. `Get Actor Of Class` (`StylizeWaterActor`)
3. Build animated UV:
4. `Time = Get Game Time In Seconds`
5. `U = 0.5 + cos(Time * 0.5) * 0.25`
6. `V = 0.5 + sin(Time * 0.5) * 0.25`
7. Call `UpdateBoatPhysics(UVPosition=(U,V), Heading=Time, SpeedNorm=1.0)`

For obstacles (once in `BeginPlay`):

- `UpdateObstacle(Index=0, UV=(0.35,0.35), RadiusUV=0.06)`
- `UpdateObstacle(Index=1, UV=(0.65,0.6), RadiusUV=0.08)`

## 5. Expected visual result

- Water plane deforms over time (WPO from simulation texture)
- Wake/ripple pattern appears around animated boat UV
- Obstacle areas affect flow/foam pattern if shader logic uses `Obstacle*`
- Depth stress geometry exists as `Stylize_DepthStep_0` to `Stylize_DepthStep_9` (folder: `TestGeometry/DepthStress`)

## 6. Quick troubleshooting

- If water is flat:
  - Confirm `M_WaterSurface_Template` uses `SimulationTexture` for displacement.
- If no simulation updates:
  - Confirm `M_WaterSim_Template` exists and has required parameter names.
  - Confirm `UpdateBoatPhysics` is called in `Tick`.
- If visuals are too strong or too weak:
  - Tune `WaveAmplitude` and `FoamIntensity` on the `Stylize_WaterActor`.
- Check Output Log for:
  - `missing SimulationMaterialTemplate`
  - `missing WaterSurfaceMaterialTemplate`

## 7. Rebuild test level automatically

```powershell
& "G:\TA_Private\Ure\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "D:\dev\INTERNAL\KIDs 5.7\KIDs.uproject" `
  -run=pythonscript `
  -script="D:/dev/INTERNAL/KIDs 5.7/Scripts/CreateStylizeTestLevel.py" `
  -unattended -nop4 -nosplash -nullrhi
```

## 8. Run automation tests

```powershell
& "G:\TA_Private\Ure\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "D:\dev\INTERNAL\KIDs 5.7\KIDs.uproject" `
  -unattended -nop4 -nosplash -nullrhi `
  -ExecCmds="Automation RunTests KIDs.Stylize" `
  -TestExit="Automation Test Queue Empty" `
  -ReportOutputPath="D:\dev\INTERNAL\KIDs 5.7\Saved\Automation\Stylize"
```

## 9. Full automation (one command)

Run the complete pipeline (build, create/verify assets, create/verify level, run tests):

```powershell
powershell -ExecutionPolicy Bypass -File "D:\dev\INTERNAL\KIDs 5.7\Scripts\RunStylizeWaterFullAutomation.ps1"
```

Optional:

- Open editor on test map when done:

```powershell
powershell -ExecutionPolicy Bypass -File "D:\dev\INTERNAL\KIDs 5.7\Scripts\RunStylizeWaterFullAutomation.ps1" -OpenEditor
```

- Skip C++ build step:

```powershell
powershell -ExecutionPolicy Bypass -File "D:\dev\INTERNAL\KIDs 5.7\Scripts\RunStylizeWaterFullAutomation.ps1" -SkipBuild
```
