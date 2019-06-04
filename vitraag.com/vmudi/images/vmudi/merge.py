import os, sys
from PIL import Image
sub_images = []
mood = ["depressed", "sad", "neutral", "happy", "elated"]
stress = ["verylow", "low", "moderate", "high", "veryhigh"]
wellbeing = ["sick", "impaired", "able", "healthy", "vigorous"]

for m in mood:
    for s in stress:
        for w in wellbeing:
            img1 = Image.open("vmudi_%s.png" % m)
            img2 = Image.open("vmudi_%s.png" % s)
            img3 = Image.open("vmudi_%s.png" % w)
            img3.paste(img2,(0,0),img2)
            img3.paste(img1, (0,0), img1)
            img3.save("vmudi_%s_%s_%s.png" % (m,s,w))
