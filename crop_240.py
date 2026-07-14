import os
from PIL import Image

p = r'd:\Lab_He_Nhung\SpaceInvaders\TouchGFX\assets\images'

def crop_and_convert(f, size):
    fp = os.path.join(p, f)
    if os.path.exists(fp):
        img = Image.open(fp).crop((0, 0, size[0], size[1]))
        img.save(fp)

for f in ['background_1.png', 'dust_1.png', 'planets_1.png', 'stars_1.png']:
    crop_and_convert(f, (240, 320))
