import unreal
import os
out_path = r"D:/dev/INTERNAL/KIDs 5.7/Saved/MaterialEditingLibrary_methods.txt"
with open(out_path, 'w', encoding='utf-8') as f:
    for n in sorted(dir(unreal.MaterialEditingLibrary)):
        f.write(n + '\n')
