# asmc_fan_control

asmc_fan_control MIN_MAX_ARGS SYSCTL_NAMES...

## MIN_MAX_ARGS
The following MIN_MAX_ARGS are required:
* --fan-min INT    Minimum speed the fan should run at
* --fan-max INT    Maximum speed the fan should run at
* --temp-min INT   Temperature at which the fan should spin at minimum speed
* --temp-max INT   Temperature at which the fan should spin at maximum speed

## SYSCTL_NAMES
List of all sysctl names to use, consisting of one or more temperatures and one fan

## Example
asmc_fan_control --fan-min 1400 --fan-max 4350 --temp-min 55 --temp-max 65 dev.asmc.0.temp.gpu_0_die dev.asmc.0.fan.0.minspeed
