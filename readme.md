# Orbrit

# TODO
- add control on simulation speed
- add prediction:
   - display current object's path
   - display other objects final position
   - add control on prediction's length
- custom "gui" system
   - easy to change
   - keyboard / arrow (controllers) / mouse friendly
   - display live information or hide on panels:
      - orbit characteristics: e, speed, period, distance to center & surface
      -
- fix orbit display on e close to 1 (not linear range of angle)
- add save restore feature
- on changing which object is followed, save current settings & restore them when re-following the object (& add reset)
- cap zoom both ways & add gui slider
- on big enough zoom, display randomized / procedural details on planets for fun?

About orbits and predicted path
- orbits with high eccentricity can probably be considered lost
- predicted path should be displayed in the reference coordinates of another object, most of the time one of the primaries being orbited.
- do we want to see objects move around their theoretical orbits (considering initial conditions), or do we want to see actualised orbits wobbling?

# Fun facts & links

- http://www.braeunig.us/space/orbmech.htm
- https://en.wikipedia.org/wiki/Semi-major_and_semi-minor_axes
- Numerical integration seems to hold up to a 10s time step. At 100s, the moon escapes the earth's orbit.
- The sun attracts the moon twice as much as the earth does, so the moon is actually orbiting the sun more than it does the earth. Chossing a satellite / primary relationship seems to depend on the desired computation (but a huge eccentricity is a good sign that no orbit is happening).