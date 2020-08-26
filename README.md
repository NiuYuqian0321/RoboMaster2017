# RoboMaster2017
Armor Detection &amp; Shooting

Mode.h decide whether the image is shown or written, it will get different fps.

CArmorDetection.cpp return the only point of the armor.

ChariotEx.cpp is reponsible for automatic control to let the shooting point at the armor center.

/*

@author:NiuYuqian

@last date:2017-08-25



judge if it is the armor:

1. the s in hsv is high enough

2. the area of the contours is in a proper range

3. the area of the contours / the area of the rect of the contours is in a proper range

4. the rect is upright

5. the angle of the rect is small enough

6. the small side of the rect , the large side of the rect

7. the large side difference / the large sidr of the rect

8. the large side of the rect / the small side of the rect

9. the distance between two center point

10. the difference of the y of the two center point 

11. color





promotion1：only process 160-270 rows in the picture.

promotion2: to avoid the light is so small that can't be seen, find contours again after draw the light by size 2

promotion3: only choose the lowest point if there are more chariots

 */
