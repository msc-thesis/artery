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
