import unreal

ASSET_PATHS = [
    "/Game/StylizeRender/Materials/M_WaterSim_Template",
    "/Game/StylizeRender/Materials/M_WaterSurface_Template",
]


def log(msg: str) -> None:
    unreal.log(f"[StylizeWaterVerify] {msg}")


def verify_material(path: str) -> bool:
    exists = unreal.EditorAssetLibrary.does_asset_exist(path)
    if not exists:
        log(f"MISSING: {path}")
        return False

    asset = unreal.load_asset(path)
    if not asset:
        log(f"FAILED_TO_LOAD: {path}")
        return False

    cls_name = asset.get_class().get_name()
    log(f"LOADED: {path} (Class={cls_name})")

    if isinstance(asset, unreal.Material):
        expr_count = unreal.MaterialEditingLibrary.get_num_material_expressions(asset)
        tex_params = unreal.MaterialEditingLibrary.get_texture_parameter_names(asset)
        scalar_params = unreal.MaterialEditingLibrary.get_scalar_parameter_names(asset)
        vector_params = unreal.MaterialEditingLibrary.get_vector_parameter_names(asset)
        log(
            f"DETAILS: Expressions={expr_count}, "
            f"TextureParams={list(tex_params)}, "
            f"ScalarParams={list(scalar_params)}, "
            f"VectorParams={list(vector_params)}"
        )
    return True


def main() -> None:
    ok = True
    for path in ASSET_PATHS:
        ok = verify_material(path) and ok

    if ok:
        log("VERIFY_OK")
    else:
        raise RuntimeError("Stylize water asset verification failed.")


if __name__ == "__main__":
    main()
