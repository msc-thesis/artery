Radio transmitter power: 10 mW = ~ 200 m radius circle aroung car

1.327.361: FOG !!!

## 2021-10-14

The LTE network works the following way:
1. When a new node enters the simulation, it searches for an eNodeB station
2. It does this by calculating the SINR (Signal to Interference & Noise Ratio) between each eNodeB and the node
3. The channel model is either urban or rural (or indoor), and either LOS (line-of-sight) or NLOS (non-line-of-sight)
4. In each of these combinations, the path loss model is valid for <10000 m (or <5000 m)
5. This means that nodes and eNodeB stations that are further away than 10 km are not valid in the model
6. Since the node calculates the distance to each eNodeB, there will usually be a station which is further away than 10 km
7. Even if the stations cover the whole area with 3 km long radii that are overlapping, the last eNodeB will throw an error
8. There is a switch that makes the model tolerate this
9. Since the node will always talk to the nearest eNodeB, and handover works correctly, this will probably not alter the model significantly.

## 2021-10-21

configure Artery for Debug mode:
```
cmake -DCMAKE_BUILD_TYPE=Debug  ..
```

configure Artery for Debug mode, with Cmdenv:
```
cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_FLAGS="-u Cmdenv" ..
```

build Artery:
cmake --build .

6 836 425

## 2021-11-04

Relevance distance and destination area are two different things:
- Relevance distance: the distance, within which the information is
    considered relevant. If a car is outside the relevance distance,
    it will not process the related message.
- Destination area: the area, where the information should be sent to.
    A car can mark the intersection behind itself the destination area.
    In this case, every car there will receive the message. But if the
    Relevance distance is smaller than the car's distance to the destination
    area, the message will be dropped.

- The destination area has a maximum size: 10 km<sup>2</sup>
- The area can have one of the following shapes:
    + rectangle
    + circle (max r: 1784 m)
    + ellipse
- The car is in the destination area by default
    + The circle is drawn around it (ellipse too)
    + When a rectangle is defined, a and b means how far the center of
        the rectangle is from the edges.
- This means that normally, the relevance distance is maximum 2.5 kms behind
    the car (In case of a 5km x 2km large rectangle)
- I calculated the direction of the car, negated it, and moved the destination
    area back by 2.5 kms, so that the car is at the end of the rectangle.
    This way the destination area is 2 km wide, and 5 km long behind the car.

## 2021-11-15

`python sumo/tools/randomTrips.py -n artery/scenarios/hybrid/mzero.net.xml -e 600 --route-file artery/scenarios/hybrid/mzero.rou.xml --validate`

## 2021-11-16

TMC
- every car that enters the simulation must register itself to the TMC
- they send a "TMC registration" message to a predefined address
- TMC saves their address in a set, and send an "ACK" message
- Cars try to register as long as they do not receive the ACK
- When ACK is received, they stop the attempts
- Everything happens over UDP
