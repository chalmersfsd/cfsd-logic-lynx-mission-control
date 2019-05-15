# cfsd-logic-lynx-mission-control
This mission control is a logic microsercvices with a small state machine inside which Rely on the main state machine.

see transaction at figure MissionControl-state-diagram.jpg

The logic of state transform is in the main thread.

The logic part for different missions should be implemented by inheriting from the super clas: MissionControl.

The virtual functions:
```
create_data_trigger()
init()
wait()
step()
abort()
```

some enums:

mission index:
```
acceleration 1
skidpad 2
trackdrive 3
braketest 4
inspection 5
```

Mission brake test:

IN 6.3 Driverless Inspection EBS Test

IN 6.3.1 The EBS performance will be tested dynamically and must demonstrate the performance described in DV 3.3.

IN 6.3.2 The test will be performed in a straight line marked with cones similar to acceleration.

IN 6.3.3 During the brake test, the vehicle must accelerate in autonomous mode up to at least 40 km/h within 20 m. From the point where the RES is triggered, the vehicle must come to a safe stop within a maximum distance of 10 m.

IN 6.3.4 In case of wet track conditions, the stopping distance will be scaled by the officials dependent on the friction level of the track


