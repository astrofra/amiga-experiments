REM Compress the music file
..\C-toolchain\miniz\bin\miniz.exe c music\jhericurl-med-mandarine-v2.mod ..\src\assets\jhericurl-med-mandarine.dat

REM Compress the graphics files
REM Cityscape
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\element_city.bin ..\src\assets\element_city.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\element_tree.bin ..\src\assets\element_tree.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\element_bridge.bin ..\src\assets\element_bridge.dat

REM Title - Place
..\C-toolchain\miniz\bin\miniz.exe c 2D\title_place.bin ..\src\assets\title_place.dat

REM Various Titles...
del ..\src\assets\mistral_title_0.dat
del ..\src\assets\mistral_title_1.dat
del ..\src\assets\mistral_title_2.dat
del ..\src\assets\mistral_title_3.dat
copy 3D\trabant\convert\mistral_title_0.bin ..\src\assets\mistral_title_0.dat
copy 3D\trabant\convert\mistral_title_1.bin ..\src\assets\mistral_title_1.dat
copy 3D\trabant\convert\mistral_title_2.bin ..\src\assets\mistral_title_2.dat
copy 3D\trabant\convert\mistral_title_3.bin ..\src\assets\mistral_title_3.dat

REM Trabant screen #0
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_ground.bin ..\src\assets\trabant_facing_ground.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_car.bin ..\src\assets\trabant_facing_car.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_car_l0.bin ..\src\assets\trabant_facing_car_l0.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_facing_car_l1.bin ..\src\assets\trabant_facing_car_l1.dat

REM Trabant screen #1
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_side_ground.bin ..\src\assets\trabant_side_ground.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\trabant_side_car.bin ..\src\assets\trabant_side_car.dat

rem Onliner screen
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\element_tower.bin ..\src\assets\element_tower.dat
..\C-toolchain\miniz\bin\miniz.exe c 3D\trabant\convert\element_city_2b.bin ..\src\assets\element_city_2b.dat

pause