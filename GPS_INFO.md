https://gpsd.gitlab.io/gpsd/NMEA.html#_gsa_gps_dop_and_active_satellites

https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)

XXGGA-GPS FIX DATA
1. time UTC
2. Lat
3. N/S
4. long
5. E/W
6. Quality Indicator
7. Num of Sat
8. Horizontal Dilution
9. Atenna Alt. MSL
10. Units of alt.
11. Geoidal seperation
12. Units of sep
13. Age of data(sec)
14. Differential ref station ID
15. Checksum

XXGLL-GEO POS
1. Lat
2. N/S
3. Long
4. E/W
5. UTC
6. Status A-valid V-void
7. FAA mode indicator
8. Checksum

XXGSA-GPS DOP and act Sat
1. Selection Mode M-manual A-automatic
2. Mode 1-no fix 2-2D 3-3D
3. ID 1
4. ID 2
5. ID 3
...
14. ID 12
15. PDOP
16. HDOP
17. VDOP
18. System ID
19. Checksum

XXGSV-Satellites In View
1. Num of GSV sen. in this group
2. Sen num
3. num of sat (leading zero)
4. Sat ID or PRN num (leading 0)
5. elevation in degrees (-90, 90)(leading 0)
6. azimuth in deg o true north (000, 359)(leading 0)
7. ?

XXRMC-Recommended Minimum Navigation Information
1. UTC
2. Status
3. Lat
4. N/S
5. Long
6. E/W
7. Speed over ground, knots
8. Track made good degrees true
9. Data, ddmmyy
10. mag var
11. E/W
12. FAA mode indicator
13. Nav Status A=auto d=differential E=estimated M=manual N=not valid S=Sim V=valid
14. Checksum

XXVTG-Track Made Good And Ground Speed
1. Course over ground, degrees true
2. T=true
3. Course over ground, deg mag
4. M=mag
5. Speed over ground, knots
6. N=knots
7. speed over ground, kph
8. K=kph
9. FAA mode indecator
10. checksum

