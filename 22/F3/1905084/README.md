In `point-to-point-dumbbell.h` please set the private member `m_routerDevices` as public.
In `tcp-westwood-plus.h` please set the private method `EstimateBW()` as protected.
Place `tcp-westwood-plus.h` and `tcp-westwood-plus.cc` in `src/internet/model`.
In `serc/internet/CMakeLists.txt` add `model/tcp-adaptive-reno.cc` and `model/tcp-adaptive-reno.cc` under the sections `set (source_files` and `set (header_files` respectively.
`1905084.cc` and `1905084.sh` go in `scratch/` and the root directory respectively.
