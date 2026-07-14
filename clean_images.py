import os
from PIL import Image

p = r'd:\Lab_He_Nhung\SpaceInvaders\TouchGFX\assets\images'

def crop_and_convert(f, size, rgb=False):
    fp = os.path.join(p, f)
    if os.path.exists(fp):
        img = Image.open(fp).crop((0, 0, size[0], size[1]))
        if rgb:
            img = img.convert('RGB')
        img.save(fp)

# Crop parallax to 240x400
for f in ['background_1.png', 'dust_1.png', 'planets_1.png', 'stars_1.png']:
    crop_and_convert(f, (240, 400), rgb=(f=='background_1.png'))

# Crop backgrounds to 240x320 and convert to RGB
for f in ['background_space.png', 'background_stars.png']:
    crop_and_convert(f, (240, 320), rgb=True)

# Delete unwanted files
to_delete = []
for f in os.listdir(p):
    if f.endswith('.png'):
        if f.startswith('nebula_') or f.startswith('background_nebula'):
            to_delete.append(f)
        elif f.startswith(('background_', 'dust_', 'planets_', 'stars_')) and any(c in f for c in ['_2', '_3', '_4', '_5']):
            to_delete.append(f)
        elif f.startswith('num_'):
            to_delete.append(f)
        elif f in ['asteroid_blue.png', 'asteroid_green.png', 'asteroid_purple.png', 'asteroid_white.png']:
            to_delete.append(f)

for f in to_delete:
    os.remove(os.path.join(p, f))
