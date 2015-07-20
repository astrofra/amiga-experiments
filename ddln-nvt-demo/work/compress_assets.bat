REM Compress the music file
..\C-toolchain\miniz\bin\miniz.exe c music\miami_vice.mod ..\src\assets\miami_vice.dat

REM Compress the graphics files
REM Cityscape
..\C-toolchain\miniz\bin\miniz.exe c 2D\element_city.bin ..\src\assets\element_city.dat

REM Trabant screen #0
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_ground.bin ..\src\assets\trabant_facing_ground.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_car.bin ..\src\assets\trabant_facing_car.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_car_l0.bin ..\src\assets\trabant_facing_car_l0.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_car_l1.bin ..\src\assets\trabant_facing_car_l1.dat

pause