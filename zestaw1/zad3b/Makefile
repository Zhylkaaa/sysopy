CC = gcc -Wall -std=c11 -ggdb

all: static shared clean

static: library.o
		ar rcs library.a library.o

shared: library.o
		$(CC) -c -fPIC library.c
		$(CC) -shared -fPIC -o library.so library.o

library.o: library.c library.h
		$(CC) -c library.c

clean:
		rm -f *.o

tests:

		./test create_table 5
		#./test create_table 2 compare_pairs a.txt:b.txt a.txt:b.txt remove_operation 0 2 remove_block 0

shared-O3: library.c library.h
		$(CC) -c -fPIC -O3 library.c
		$(CC) -shared -fPIC -O3 -o library.so library.o

time-tests-shared-O3: shared measure_time.c
		make shared-O3
		$(CC) -O3 -o test measure_time.c -L. library.so -I .

		echo "shared library usage with -O3 flag\n" >> raport3b.txt

		./test small_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt >> raport3b.txt
		./test medium_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt sa5.txt:sb5.txt sa6.txt:sb6.txt sa7.txt:sb7.txt sa8.txt:sb8.txt sa9.txt:sb9.txt sa10.txt:sb10.txt sa11.txt:sb11.txt >> raport3b.txt
		./test big_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt sa5.txt:sb5.txt sa6.txt:sb6.txt sa7.txt:sb7.txt sa8.txt:sb8.txt sa9.txt:sb9.txt sa10.txt:sb10.txt sa11.txt:sb11.txt sa12.txt:sb12.txt sa13.txt:sb13.txt sa14.txt:sb14.txt sa15.txt:sb15.txt sa16.txt:sb16.txt sa17.txt:sb17.txt sa18.txt:sb18.txt sa19.txt:sb19.txt sa20.txt:sb20.txt sa21.txt:sb21.txt sa22.txt:sb22.txt sa23.txt:sb23.txt sa24.txt:sb24.txt sa25.txt:sb25.txt sa26.txt:sb26.txt sa27.txt:sb27.txt sa28.txt:sb28.txt sa29.txt:sb29.txt >> raport3b.txt

		./test small_halfdif ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt >> raport3b.txt
		./test medium_halfdif ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt ma5.txt:mb5.txt ma6.txt:mb6.txt ma7.txt:mb7.txt ma8.txt:mb8.txt ma9.txt:mb9.txt ma10.txt:mb10.txt ma11.txt:mb11.txt >> raport3b.txt
		./test big_halfdif ma0.txt:mb0.txt ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt ma5.txt:mb5.txt ma6.txt:mb6.txt ma7.txt:mb7.txt ma8.txt:mb8.txt ma9.txt:mb9.txt ma10.txt:mb10.txt ma11.txt:mb11.txt ma12.txt:mb12.txt ma13.txt:mb13.txt ma14.txt:mb14.txt ma15.txt:mb15.txt ma16.txt:mb16.txt ma17.txt:mb17.txt ma18.txt:mb18.txt ma19.txt:mb19.txt ma20.txt:mb20.txt ma21.txt:mb21.txt ma22.txt:mb22.txt ma23.txt:mb23.txt ma24.txt:mb24.txt ma25.txt:mb25.txt ma26.txt:mb26.txt ma27.txt:mb27.txt ma28.txt:mb28.txt ma29.txt:mb29.txt >> raport3b.txt

		./test small_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt >> raport3b.txt
		./test medium_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt ha5.txt:hb5.txt ha6.txt:hb6.txt ha7.txt:hb7.txt ha8.txt:hb8.txt ha9.txt:hb9.txt ha10.txt:hb10.txt ha11.txt:hb11.txt >> raport3b.txt
		./test big_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt ha5.txt:hb5.txt ha6.txt:hb6.txt ha7.txt:hb7.txt ha8.txt:hb8.txt ha9.txt:hb9.txt ha10.txt:hb10.txt ha11.txt:hb11.txt ha12.txt:hb12.txt ha13.txt:hb13.txt ha14.txt:hb14.txt ha15.txt:hb15.txt ha16.txt:hb16.txt ha17.txt:hb17.txt ha18.txt:hb18.txt ha19.txt:hb19.txt ha20.txt:hb20.txt ha21.txt:hb21.txt ha22.txt:hb22.txt ha23.txt:hb23.txt ha24.txt:hb24.txt ha25.txt:hb25.txt ha26.txt:hb26.txt ha27.txt:hb27.txt ha28.txt:hb28.txt ha29.txt:hb29.txt >> raport3b.txt

shared-O0: library.c library.h
		$(CC) -c -fPIC -O0 library.c
		$(CC) -shared -fPIC -O0 -o library.so library.o

time-tests-shared-O0: shared measure_time.c
		make shared-O0
		$(CC) -O0 -o test measure_time.c -L. library.so -I .

		echo "shared library usage with -O0 flag\n" >> raport3b.txt

		./test small_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt >> raport3b.txt
		./test medium_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt sa5.txt:sb5.txt sa6.txt:sb6.txt sa7.txt:sb7.txt sa8.txt:sb8.txt sa9.txt:sb9.txt sa10.txt:sb10.txt sa11.txt:sb11.txt >> raport3b.txt
		./test big_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt sa5.txt:sb5.txt sa6.txt:sb6.txt sa7.txt:sb7.txt sa8.txt:sb8.txt sa9.txt:sb9.txt sa10.txt:sb10.txt sa11.txt:sb11.txt sa12.txt:sb12.txt sa13.txt:sb13.txt sa14.txt:sb14.txt sa15.txt:sb15.txt sa16.txt:sb16.txt sa17.txt:sb17.txt sa18.txt:sb18.txt sa19.txt:sb19.txt sa20.txt:sb20.txt sa21.txt:sb21.txt sa22.txt:sb22.txt sa23.txt:sb23.txt sa24.txt:sb24.txt sa25.txt:sb25.txt sa26.txt:sb26.txt sa27.txt:sb27.txt sa28.txt:sb28.txt sa29.txt:sb29.txt >> raport3b.txt

		./test small_halfdif ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt >> raport3b.txt
		./test medium_halfdif ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt ma5.txt:mb5.txt ma6.txt:mb6.txt ma7.txt:mb7.txt ma8.txt:mb8.txt ma9.txt:mb9.txt ma10.txt:mb10.txt ma11.txt:mb11.txt >> raport3b.txt
		./test big_halfdif ma0.txt:mb0.txt ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt ma5.txt:mb5.txt ma6.txt:mb6.txt ma7.txt:mb7.txt ma8.txt:mb8.txt ma9.txt:mb9.txt ma10.txt:mb10.txt ma11.txt:mb11.txt ma12.txt:mb12.txt ma13.txt:mb13.txt ma14.txt:mb14.txt ma15.txt:mb15.txt ma16.txt:mb16.txt ma17.txt:mb17.txt ma18.txt:mb18.txt ma19.txt:mb19.txt ma20.txt:mb20.txt ma21.txt:mb21.txt ma22.txt:mb22.txt ma23.txt:mb23.txt ma24.txt:mb24.txt ma25.txt:mb25.txt ma26.txt:mb26.txt ma27.txt:mb27.txt ma28.txt:mb28.txt ma29.txt:mb29.txt >> raport3b.txt

		./test small_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt >> raport3b.txt
		./test medium_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt ha5.txt:hb5.txt ha6.txt:hb6.txt ha7.txt:hb7.txt ha8.txt:hb8.txt ha9.txt:hb9.txt ha10.txt:hb10.txt ha11.txt:hb11.txt >> raport3b.txt
		./test big_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt ha5.txt:hb5.txt ha6.txt:hb6.txt ha7.txt:hb7.txt ha8.txt:hb8.txt ha9.txt:hb9.txt ha10.txt:hb10.txt ha11.txt:hb11.txt ha12.txt:hb12.txt ha13.txt:hb13.txt ha14.txt:hb14.txt ha15.txt:hb15.txt ha16.txt:hb16.txt ha17.txt:hb17.txt ha18.txt:hb18.txt ha19.txt:hb19.txt ha20.txt:hb20.txt ha21.txt:hb21.txt ha22.txt:hb22.txt ha23.txt:hb23.txt ha24.txt:hb24.txt ha25.txt:hb25.txt ha26.txt:hb26.txt ha27.txt:hb27.txt ha28.txt:hb28.txt ha29.txt:hb29.txt >> raport3b.txt

shared-Os: library.c library.h
		$(CC) -c -fPIC -Os library.c
		$(CC) -shared -fPIC -Os -o library.so library.o

time-tests-shared-Os: shared measure_time.c
		make shared-Os
		$(CC) -Os -o test measure_time.c -L. library.so -I .

		echo "shared library usage with -Os flag\n" >> raport3b.txt

		./test small_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt >> raport3b.txt
		./test medium_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt sa5.txt:sb5.txt sa6.txt:sb6.txt sa7.txt:sb7.txt sa8.txt:sb8.txt sa9.txt:sb9.txt sa10.txt:sb10.txt sa11.txt:sb11.txt >> raport3b.txt
		./test big_similar sa0.txt:sb0.txt sa1.txt:sb1.txt sa2.txt:sb2.txt sa3.txt:sb3.txt sa4.txt:sb4.txt sa5.txt:sb5.txt sa6.txt:sb6.txt sa7.txt:sb7.txt sa8.txt:sb8.txt sa9.txt:sb9.txt sa10.txt:sb10.txt sa11.txt:sb11.txt sa12.txt:sb12.txt sa13.txt:sb13.txt sa14.txt:sb14.txt sa15.txt:sb15.txt sa16.txt:sb16.txt sa17.txt:sb17.txt sa18.txt:sb18.txt sa19.txt:sb19.txt sa20.txt:sb20.txt sa21.txt:sb21.txt sa22.txt:sb22.txt sa23.txt:sb23.txt sa24.txt:sb24.txt sa25.txt:sb25.txt sa26.txt:sb26.txt sa27.txt:sb27.txt sa28.txt:sb28.txt sa29.txt:sb29.txt >> raport3b.txt

		./test small_halfdif ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt >> raport3b.txt
		./test medium_halfdif ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt ma5.txt:mb5.txt ma6.txt:mb6.txt ma7.txt:mb7.txt ma8.txt:mb8.txt ma9.txt:mb9.txt ma10.txt:mb10.txt ma11.txt:mb11.txt >> raport3b.txt
		./test big_halfdif ma0.txt:mb0.txt ma1.txt:mb1.txt ma2.txt:mb2.txt ma3.txt:mb3.txt ma4.txt:mb4.txt ma5.txt:mb5.txt ma6.txt:mb6.txt ma7.txt:mb7.txt ma8.txt:mb8.txt ma9.txt:mb9.txt ma10.txt:mb10.txt ma11.txt:mb11.txt ma12.txt:mb12.txt ma13.txt:mb13.txt ma14.txt:mb14.txt ma15.txt:mb15.txt ma16.txt:mb16.txt ma17.txt:mb17.txt ma18.txt:mb18.txt ma19.txt:mb19.txt ma20.txt:mb20.txt ma21.txt:mb21.txt ma22.txt:mb22.txt ma23.txt:mb23.txt ma24.txt:mb24.txt ma25.txt:mb25.txt ma26.txt:mb26.txt ma27.txt:mb27.txt ma28.txt:mb28.txt ma29.txt:mb29.txt >> raport3b.txt

		./test small_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt >> raport3b.txt
		./test medium_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt ha5.txt:hb5.txt ha6.txt:hb6.txt ha7.txt:hb7.txt ha8.txt:hb8.txt ha9.txt:hb9.txt ha10.txt:hb10.txt ha11.txt:hb11.txt >> raport3b.txt
		./test big_dif ha0.txt:hb0.txt ha1.txt:hb1.txt ha2.txt:hb2.txt ha3.txt:hb3.txt ha4.txt:hb4.txt ha5.txt:hb5.txt ha6.txt:hb6.txt ha7.txt:hb7.txt ha8.txt:hb8.txt ha9.txt:hb9.txt ha10.txt:hb10.txt ha11.txt:hb11.txt ha12.txt:hb12.txt ha13.txt:hb13.txt ha14.txt:hb14.txt ha15.txt:hb15.txt ha16.txt:hb16.txt ha17.txt:hb17.txt ha18.txt:hb18.txt ha19.txt:hb19.txt ha20.txt:hb20.txt ha21.txt:hb21.txt ha22.txt:hb22.txt ha23.txt:hb23.txt ha24.txt:hb24.txt ha25.txt:hb25.txt ha26.txt:hb26.txt ha27.txt:hb27.txt ha28.txt:hb28.txt ha29.txt:hb29.txt >> raport3b.txt

clean-library:
		rm -f *.so
		rm -f *.a