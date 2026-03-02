import unreal
names = [n for n in dir(unreal.MaterialEditingLibrary) if 'expression' in n.lower() or 'material' in n.lower()]
for n in sorted(names):
    unreal.log('[MatEditAPI] ' + n)
