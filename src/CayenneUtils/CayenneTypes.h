/*
The MIT License(MIT)

Cayenne MQTT Client Library
Copyright (c) 2016 myDevices

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CAYENNETYPES_h
#define _CAYENNETYPES_h

#define UNDEFINED "null"

// Data types
#define ABSOLUTE_HUMIDITY "abs_hum" // Absolute Humidity
#define ABSORBED_RADIATION "absrb_rad" // Absorbed Radiation
#define ACCELERATION_GX "gx" // Acceleration.gx axis
#define ACCELERATION_GY "gy" // Acceleration.gy axis
#define ACCELERATION_GZ "gz" // Acceleration.gz axis
#define ALTITUDE "alt" // Altitude
#define AMOUNT_SUBSTANCE "amount" // Amount of substance
#define AREA "area" // Area
#define BAROMETRIC_PRESSURE "bp" // Barometric pressure
#define BATTERY "batt" // Battery
#define BIOMETRIC "bio" // Biometric
#define BLOOD "blood" // Blood Count
#define BYTES "bytes" // Bytes
#define CAPACITANCE "cap" // Capacitance
#define CO2 "co2" // Carbon Dioxide
#define CHARGE "charge" // Charge
#define CHOLESTEROL "chol" // Cholesterol
#define COLOR "color" // Color
#define CONDUCTANCE "conduct" // Conductance
#define CPU "cpu" // CPU
#define CURRENT "current" // Current
#define CURRENT_DENSITY "current_density" // Current density
#define DENSITY "density" // Density
#define DISTANCE "dist" // Distance
#define EFFECTIVE_RADATION "eff_rad" // Effective Radiation
#define FORCE "force" // Force
#define FREQUENCY "freq" // Frequency
#define GAS "gas" // Gas
#define GLUCOSE "glucose" // Glucose
#define GPS "gps" // GPS
#define GRAVITY_X "grav_x" // Gravity.x axis
#define GRAVITY_Y "grav_y" // Gravity.y axis
#define GRAVITY_Z "grav_z" // Gravity.z axis
#define GYRO_X "gyro_x" // Gyroscope.rate of rotation around x axis
#define GYRO_Y "gyro_y" // Gyroscope.rate of rotation around y axis
#define GYRO_Z "gyro_z" // Gyroscope.rate of rotation around z axis
#define HUMIDITY "hum" // Humdity
#define HVAC_HUMIDITY "hvac_hum" // HVAC.Humdity
#define IMAGE "image" // Image
#define IMPEDANCE "imped" // Impedance
#define INDUCTANCE "induct" // Inductance
#define INK_BLACK "ink_blk" // Ink Levels.Black
#define INK_CYAN "ink_cya" // Ink Levels.Cyan
#define INK_MEGENTA "ink_mag" // Ink Levels.Magenta
#define INK_YELLOW "ink_yel" // Ink Levels.Yellow
#define INTRUSION "intrusion" // Intrusion
#define IONIZING_RADIATION "ion_rad" // Ionizing Radiation
#define LENGTH "len" // Length
#define LIGHTING_SENSE "lighting_sense" // Lighting
#define LINEAR_ACCEL_X "lin_acc_x" // Proximity.x axis
#define LINEAR_ACCEL_Y "lin_acc_y" // Proximity.y axis
#define LINEAR_ACCEL_Z "lin_acc_z" // Proximity.z axis
#define LIQUID "liquid" // Liquid
#define LOCATION_LAT "loc_lat" // Location.Latitude
#define LOCATION_LONG "loc_lon" // Location.Longitude
#define LUMINOSITY "lum" // Luminosity
#define MAGNETIC_STRENGTH "mag_str" // Magnetic field strength H
#define MAGNETIC_AXIS_X "mag_x" // Magnetic field.x axis
#define MAGNETIC_AXIS_Y "mag_y" // Magnetic field.y axis
#define MAGNETIC_AXIS_Z "mag_z" // Magnetic field.z axis
#define MAGNETIC_FLUX_DENSITY "mag_flux" // Magnetic flux density B
#define MASS "mass" // Mass
#define MEMORY "mem" // Memory
#define MOTION "motion" // Motion
#define OIL "oil" // Oil
#define ORIENT_AZIMUTH "ori_azim" // Orientation.Azimuth
#define ORIENT_PITCH "ori_pitch" // Orientation.Pitch
#define ORIENT_ROLL "ori_roll" // Orientation.Roll
#define ACIDITY "acid" // pH-Acidity
#define POWER "pow" // Power
#define PRESSURE "press" // Pressure
#define PROXIMITY "prox" // Proximity
#define RADIOACTIVITY "rad" // Radioactivity
#define EXPOSURE_RADIATION "expo_rad" // Radiation Exposure
#define RELATIVE_HUMIDITY "rel_hum" // Relative Humidity
#define RESISTANCE "res" // Resistance
#define ROTATION "rot" // Rotation
#define ROTATION_SCALAR "rot_scal" // Rotation Vector.scalar
#define ROTATION_X "rot_x" // Rotation Vector.x axis
#define ROTATION_Y "rot_y" // Rotation Vector.y axis
#define ROTATION_Z "rot_z" // Rotation Vector.z axis
#define SEISMOMETER "seis" // Seismometer
#define SIGNAL_STRENGTH "sig_str" // Signal Strength
#define SMOKE "smoke" // Smoke
#define SOLID_VOLUME "solid_vol" // Solid Volume
#define SOUND "sound" // Sound
#define SPECIFIC_HUMIDITY "spec_hum" // Specific Humidity
#define SPEED "speed" // Speed
#define STEPS "steps" // Steps
#define STORAGE "storage" // Storage
#define STRESS "stress" // Stress
#define TEMPERATURE "temp" // Temperature
#define TIME "time" // Time
#define TORQUE "torq" // Torque
#define TURBIDITY "turb" // Turbidity
#define ULTRASONIC "ultra" // Ultrasonic
#define VELOCITY "velo" // Velocity
#define VISCOSITY "visco" // Viscosity
#define VOLTAGE "voltage" // Voltage
#define VOLUME "vol" // Volume
#define WATER "h20" // Water
#define WAVELENGTH "wave" // Wavelength
#define WEIGHT "weight" // Weight

#define MAX_TYPE_LENGTH 24

// Measurement units
#define KGM3 "kgm3" // Kilograms per cubic meter
#define RAD "rad" // Rad
#define GRAY "gy" // Gray
#define METER_PER_SEC_SQ "ms2" // Meters per second squared
#define METER "m" // Meters
#define FEET "ft" // Feet
#define MOLE "mol" // Mole
#define METER2 "m2" // Square meter
#define PASCAL "pa" // Pascal
#define HECTOPASCAL "hpa" // Hecto Pascal
#define PERCENT "p" // % (0 to 100)
#define BYTE_ARRAY "null" // Byte Array
#define CELLS_MM3 "cmm" // Cells by cubic millimeter
#define BIT "bit" // Bits
#define BYTE "byte" // Bytes
#define KB_BYTE "kb" // Kilobytes
#define MB_BYTE "mb" // Megabytes
#define GB_BYTE "gb" // Gigabytes
#define TB_BYTE "tb" // Terabytes
#define FARAD "farad" // Farad
#define PPM "ppm" // Parts per milliion
#define UNITS_MICROMOLE "wmoco2" // Units of Micromole
#define COULOMB "q" // Coulomb
#define MMOL_L "mmol" // Millimoles/liter
#define MG_DL "mgdl" // Milligrams/deciliter
#define RGB "null" // RGB
#define CYMK "null" // CYMK
#define HEX "null" // Hexadecimal
#define SIEMEN "s" // Siemen
#define AMP "a" // Ampere
#define AMP_2_METER "am2" // Ampere per squre meter
#define ROENTGEN "roent" // Roentgen
#define SIEVERT "sv" // Sievert
#define SIEVERT_HOUR "svph" // SieVert per Hour
#define NEWTON "null" // Newtons
#define METRIC_FORCE "force" // Metric
#define HERTZ "hz" // Hertz
#define METER3 "m3" // Cubic meters
#define GPS "gps" // Global Positioning System
#define UTM "utm" // Universal Transverse Mercator
#define NEWTON_PER_KG "nkg" // Newtons per kilogram
#define ROTATION "rot" // Rotation speed
#define OHM "ohm" // Ohm
#define HENRY "h" // Henry
#define DIGITAL "d" // Digital (0/1)
#define ELECTRON_VOLT "ev" // Electron Volts
#define ERGS "erg" // Ergs
#define JOULE "j" // Joules
#define LOW_HIGH "null" // Low/High
#define LUX "lux" // Lux
#define VOLTS "v" // Volts
#define RATIO "r" // Ratio
#define LITER "l" // Liter
#define GALLON "gal" // Gallon
#define OUNCE "oz" // Ounce
#define CUBIC_CENT "cc" // Cubic centimeter
#define LATITUDE "lat" // Latitude
#define LONGITUDE "long" // Longitude
#define AMP_METER "ampm" // Amperes per meter
#define TESLA "tesla" // Tesla
#define NEWTON_METERS_AMP "nma" // Newton-meters per ampere
#define KILOGRAM "kg" // Kilogram
#define BARREL "bbl" // Oil Barrel
#define DEGREE "deg" // Degree Angle
#define ACIDITY "acid" // Acidity
#define WATT "w" // Watts
#define BAR "bar" // Bar
#define TECH_ATMO "at" // Technical atmosphere
#define STD_ATMO "atm" // Standard atmosphere
#define TORR "torr" // Torr
#define PSI "psi" // Pounds per square inch
#define MILLIMETER "mm" // Millimeter
#define CENTIMETER "cm" // Centimeter
#define BECQUEREL "bq" // Becquerel
#define CURIE "ci" // Curie
#define COULOMB_PER_KG "ckg" // Coulomb/Kilogram
#define REL_HUMIDITY "rel_hum" // Relative Humidity
#define RPM "rpm" // Revolutions per minute
#define RPMS "rpms" // Revolutions per second
#define RADIAN_PER_SEC "radianps" // Radians per second
#define ROT_SCAL "null" // Cos(0/2)
#define ROT_X "null" // X * sin (0/2)
#define ROT_Y "null" // Y * sin (0/2)
#define ROT_Z "null" // Z * sin (0/2)
#define MICROS_PER_SEC "micps" // Microns (micrometers) /second,
#define SPECTRAL_AMP "cmhz" // Spectral Amplitude
#define DBM "dbm" // Decibels per milliwatt
#define PHOTODIODE "pz" // Photodiode
#define KILOELEC_VOLT "kev" // Kiloelectron Volts
#define CUBIC_METER "m3" // Cubic meter
#define G_PER_KG "gkg" // Grams/Kilograms
#define KM_PER_H "kmh" // Kilometer per hour
#define MPH "mph" // Miles per hour
#define FAHRENHEIT "f" // Fahrenheit
#define CELSIUS "c" // Celsius
#define KELVIN "k" // Kelvin
#define SECOND "sec" // Second
#define MILLISECOND "msec" // Milliseconds
#define MINUTE "min" // minute
#define HOUR "hour" // hour
#define DAY "day" // day
#define MONTH "month" // month
#define YEAR "year" // year
#define NEWTON_METER "newtm" // Newton-meter
#define NEPHELOMETRIC_TURBIDITY_UNIT "ntu" // Nephelometric Turbidity Unit
#define FORMAZIN_TURBIDITY_UNIT "ftu" // Formazin Turbidity Unit
#define KILOHERTZ "khz" // Kilohertz
#define METER_PER_SEC "mps" // Meters per second squared
#define MILLIPASCAL_SEC "mpas" // Millipascal-second
#define GPM "gpm" // Gallons per minute
#define CUBIC_FEET_SEC "cfs" // Cubic feet per second
#define POUND "lbs" // Pounds

#define MAX_UNIT_LENGTH 24

#endif