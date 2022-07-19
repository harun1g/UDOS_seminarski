#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <numeric>


using namespace cv;
using namespace std;
const double  PI = atan(1)*4;


double average(std::vector<double> const& v){
    if(v.empty()){
        return 0;
    }

    auto const count = static_cast<float>(v.size());
    return std::reduce(v.begin(), v.end()) / count;
}

Point centroid(vector<Point> kontura){
    Moments mu;
    mu = moments( kontura, false );
    Point mc;
    mc = Point( int(mu.m10/mu.m00) , int(mu.m01/mu.m00) );
    return mc;
}

double cirkularnost(vector<Point> kontura){
    double obim_lista;
    double povrsina_lista;
    double povrsina_kruga;
    double r;
    double cirkularnost;

    obim_lista = arcLength( kontura, false);
    r=obim_lista/(2*PI);

    povrsina_lista=contourArea( kontura, false);
    povrsina_kruga=r*r*PI;

    cirkularnost=povrsina_lista/povrsina_kruga;

    return cirkularnost;
}

double konveksnost(vector<Point> kontura){
    double obim_lista;
    double obim_konveksnog_oblika;
    double konveksnost;

    obim_lista = arcLength( kontura, false);
    vector<Point> konveksni_oblik;
    convexHull( kontura, konveksni_oblik );
    obim_konveksnog_oblika=arcLength(konveksni_oblik, false);

    konveksnost=obim_konveksnog_oblika/obim_lista;

    return konveksnost;
}

double ispunjenost(vector<Point> kontura){
    double ispunjenost;
    double povrsina_lista;
    double povrsina_konkanvog_oblika;

    povrsina_lista=contourArea( kontura, false);
    vector<Point> konveksni_oblik;
    convexHull( kontura, konveksni_oblik );
    povrsina_konkanvog_oblika=contourArea(konveksni_oblik,false);

    ispunjenost=povrsina_lista/povrsina_konkanvog_oblika;

    return ispunjenost;
}

auto data_set(string path1, int broj_slika, int th, bool prikaz){
    vector<double> c; // cirkularnost
    vector<double> k; // konveksnost
    vector<double> is; // ispunjenost

    vector<vector<double>> parametri;

    Mat ulaz,ulaz1;
    Mat maska;
    Mat element = getStructuringElement( MORPH_CROSS,
                                  Size( 4, 4 ),
                                  Point( -1, -1 ) );
    vector<Mat> biljka1_original;
    vector<Mat> binarne;
    string path = "1\\1";
    vector<Mat> slika_konture;  /* Cilj izdvojiti konturu lista */
    vector<vector<Point> > konture; /* Sadrzi sve konture sa binarne slike */
    vector<Point> kontura_lista; /* Sadrzi samo konturu lista */

    for(int i = 1; i <= broj_slika; i++){
         string path = path1;
         path = path + "-" + to_string(i) + ".jpg";
         ulaz = imread(path);


         biljka1_original.push_back(ulaz);
         cvtColor(ulaz, ulaz, COLOR_BGR2GRAY);
//         imshow("gray scale",ulaz);
//         waitKey();
         threshold(ulaz,maska,th,255,THRESH_BINARY_INV);
//         imshow("threshold",maska);
//         waitKey();

         /* Morfoloske operacije u cilju otklanjanja sto veceg broja beskorisnih kontura */
         erode(maska, maska, element);
         erode(maska, maska, element);
         dilate(maska, maska, element);
         binarne.push_back(maska);
//         imshow("Maska",  maska);
//         waitKey();

         vector<Vec4i> hijearhija;

         findContours( maska, konture, hijearhija, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE );

             for(size_t i=0;i<konture.size();i++){

                 /* Najveca kontura je kontura lista, ostale zanemariti */
                 if(konture.at(i).size()>500){
                     kontura_lista=konture.at(i);

                     /* Generisanje bijele slike istih dimenzija kao originalna */
                     Mat slika1(Size(ulaz.cols,ulaz.rows), CV_8UC3, Scalar(255, 255, 255));

                     Scalar color1 = Scalar(0, 0, 0 );
                     /* Iscrtavanje samo konture lista */
                     drawContours( slika1, konture, (int)i, color1 ,1);

                     /* Slika koja prikazuje konturu lista */
                     slika_konture.push_back(slika1);

                     /* Racunanje parametara za biljku za koristeni set slika */
                     c.push_back(cirkularnost(kontura_lista));
                     k.push_back(konveksnost(kontura_lista));
                     is.push_back(ispunjenost(kontura_lista));
                 }
             }
    }

    if(prikaz){
        for(int i=1; i<=broj_slika;i++){
            imshow("konture "+to_string(i),slika_konture.at(i-1));
            waitKey();
            cout<<"Kontura/list "+to_string(i)<<std::endl;
            cout<<"cirkularnost "<<c.at(i-1)<<std::endl;
            cout<<"konveksnost  "<<k.at(i-1)<<std::endl;
            cout<<"ispunjenost "<<is.at(i-1)<<std::endl;
        }
    }

    parametri.push_back(c);
    parametri.push_back(k);
    parametri.push_back(is);

    return parametri;
}

auto boxPlot(vector<double> niz){
    double median;
    double treci_kvartal, prvi_kvartal;
    double whisker_max, whisker_min;
    double iqr;
    vector<double> granice;

    sort(niz.begin(), niz.end());
    int n=niz.size();

    if(n%2!=0){ /*neparan*/
        median=niz.at(int(n/2));
        prvi_kvartal=niz.at(int(n/4));
        treci_kvartal=niz.at(int(n/2)+int(n/4));
    }
    else {
        median=(niz.at(int(n/2)) +niz.at(int(n/2)-1))/2;
        prvi_kvartal=niz.at(int(n/4));
        treci_kvartal=niz.at(int(n/2)+int(n/4)+1);
    }

    iqr= treci_kvartal-prvi_kvartal;

    whisker_max=treci_kvartal+0.6*iqr;
    whisker_min=prvi_kvartal-0.6*iqr;

    granice.push_back(whisker_min);
    granice.push_back(whisker_max);

    return granice;
}
class biljka {
  public:
    vector<double> opseg_c;
    vector<double> opseg_k;
    vector<double> opseg_i;
    string ime_biljke;
};

vector<string> klasifikacija(vector<biljka> sve_biljke, vector<vector<double>> parametri){
    vector<string> biljka;

        /* c_uslov = true ako parametar za cirkularnost pripada opsegu za određenu biljku */

        for(int i=0;i<parametri.at(0).size();i++){
            for(int j=0;j<sve_biljke.size();j++){
            auto x=sve_biljke.at(j);
            bool c_uslov=parametri.at(0).at(i)>=x.opseg_c.at(0) && parametri.at(0).at(i)<=x.opseg_c.at(1);
            bool k_uslov=parametri.at(1).at(i)>=x.opseg_k.at(0) && parametri.at(1).at(i)<=x.opseg_k.at(1);
            bool i_uslov=parametri.at(2).at(i)>=x.opseg_i.at(0) && parametri.at(2).at(i)<=x.opseg_i.at(1);
            if(c_uslov && k_uslov && i_uslov){
                biljka.push_back(x.ime_biljke);
                cout<<"Slika "<<i+1<<" "<<x.ime_biljke<<std::endl;

            }
        }
    }

    return biljka;
}


int main(int argc, char *argv[])
{

    ///     Dataset - Biljka 1 : Grab
    auto parametri_biljka1=data_set("1\\1",16,145 , false);

    vector<double> cirkularnost_b1(parametri_biljka1.at(0));
    vector<double> konveksnost_b1(parametri_biljka1.at(1));
    vector<double> ispunjenost_b1(parametri_biljka1.at(2));

    double cirkularnost_b1_average=average(parametri_biljka1.at(0));
    double konveksnost_b1_average=average(parametri_biljka1.at(1));
    double ispunjenost_b1_average=average(parametri_biljka1.at(2));

    cout<<"Prosjecna cirkularnost za biljku 1: "<<cirkularnost_b1_average<<std::endl;
    cout<<"Prosjecna konveksnost za biljku 1: "<<konveksnost_b1_average<<std::endl;
    cout<<"Prosjecna ispunjenost za biljku 1: "<<ispunjenost_b1_average<<std::endl;

    auto opseg_c_b1=boxPlot(parametri_biljka1.at(0));
    auto opseg_k_b1=boxPlot(parametri_biljka1.at(0));


    ///     Dataset - Biljka 2 : Obična kalina

    auto parametri_biljka2=data_set("2\\2",20,145, false );

    vector<double> cirkularnost_b2(parametri_biljka2.at(0));
    vector<double> konveksnost_b2(parametri_biljka2.at(1));
    vector<double> ispunjenost_b2(parametri_biljka2.at(2));

    double cirkularnost_b2_average=average(parametri_biljka2.at(0));
    double konveksnost_b2_average=average(parametri_biljka2.at(1));
    double ispunjenost_b2_average=average(parametri_biljka2.at(2));

    cout<<"Prosjecna cirkularnost za biljku 2: "<<cirkularnost_b2_average<<std::endl;
    cout<<"Prosjecna konveksnost za biljku 2: "<<konveksnost_b2_average<<std::endl;
    cout<<"Prosjecna ispunjenost za biljku 2: "<<ispunjenost_b2_average<<std::endl;



    ///     Dataset - Biljka 3 : Javor

    auto parametri_biljka3=data_set("3\\3",9,145,false );

    vector<double> cirkularnost_b3(parametri_biljka3.at(0));
    vector<double> konveksnost_b3(parametri_biljka3.at(1));
    vector<double> ispunjenost_b3(parametri_biljka3.at(2));

    double cirkularnost_b3_average=average(parametri_biljka3.at(0));
    double konveksnost_b3_average=average(parametri_biljka3.at(1));
    double ispunjenost_b3_average=average(parametri_biljka3.at(2));

    cout<<"Prosjecna cirkularnost za biljku 3: "<<cirkularnost_b3_average<<std::endl;
    cout<<"Prosjecna konveksnost za biljku 3: "<<konveksnost_b3_average<<std::endl;
    cout<<"Prosjecna ispunjenost za biljku 3: "<<ispunjenost_b3_average<<std::endl;


    ///     Dataset - Biljka 4 : Hrast lužnjak

    auto parametri_biljka4=data_set("4\\4",14,135, false );

    vector<double> cirkularnost_b4(parametri_biljka4.at(0));
    vector<double> konveksnost_b4(parametri_biljka4.at(1));
    vector<double> ispunjenost_b4(parametri_biljka4.at(2));

    double cirkularnost_b4_average=average(parametri_biljka4.at(0));
    double konveksnost_b4_average=average(parametri_biljka4.at(1));
    double ispunjenost_b4_average=average(parametri_biljka4.at(2));

    cout<<"Prosjecna cirkularnost za biljku 4: "<<cirkularnost_b4_average<<std::endl;
    cout<<"Prosjecna konveksnost za biljku 4: "<<konveksnost_b4_average<<std::endl;
    cout<<"Prosjecna ispunjenost za biljku 4: "<<ispunjenost_b4_average<<std::endl;


/// Kreiranje klasa za određene biljke i inicijalizacija parametara dobijenih boxPlot funckijom
    // BILJKA 1 - grab
    biljka grab;
    grab.ime_biljke="Grab";
    grab.opseg_c=boxPlot(cirkularnost_b1);
    grab.opseg_k=boxPlot(konveksnost_b1);
    grab.opseg_i=boxPlot(ispunjenost_b1);

    cout<<grab.ime_biljke<<" opseg cirkularnosti: "<<grab.opseg_c.at(0)<< " "<<grab.opseg_c.at(1)<<std::endl;
    cout<<grab.ime_biljke<<" opseg konveksnosti: "<<grab.opseg_k.at(0)<< " "<<grab.opseg_k.at(1)<<std::endl;
    cout<<grab.ime_biljke<<" opseg ispunjenosti: "<<grab.opseg_i.at(0)<< " "<<grab.opseg_i.at(1)<<std::endl;

    // BILJKA 2 - kalina
    biljka kalina;
    kalina.ime_biljke="Kalina";
    kalina.opseg_c=boxPlot(cirkularnost_b2);
    kalina.opseg_k=boxPlot(konveksnost_b2);
    kalina.opseg_i=boxPlot(ispunjenost_b2);

    cout<<"Kalina opseg cirkularnosti: "<<kalina.opseg_c.at(0)<< " "<<kalina.opseg_c.at(1)<<std::endl;
    cout<<"Kalina opseg konveksnosti:  "<<kalina.opseg_k.at(0)<< " "<<kalina.opseg_k.at(1)<<std::endl;
    cout<<"Kalina opseg ispunjenosti:  "<<kalina.opseg_i.at(0)<< " "<<kalina.opseg_i.at(1)<<std::endl;

    // BILJKA 3 - javor
    biljka javor;
    javor.ime_biljke="Javor";
    javor.opseg_c=boxPlot(cirkularnost_b3);
    javor.opseg_k=boxPlot(konveksnost_b3);
    javor.opseg_i=boxPlot(ispunjenost_b3);

    cout<<"Javor opseg cirkularnosti: "<<javor.opseg_c.at(0)<< " "<<javor.opseg_c.at(1)<<std::endl;
    cout<<"Javor opseg konveksnosti:  "<<javor.opseg_k.at(0)<< " "<<javor.opseg_k.at(1)<<std::endl;
    cout<<"Javor opseg ispunjenosti:  "<<javor.opseg_i.at(0)<< " "<<javor.opseg_i.at(1)<<std::endl;


    // BILJKA 4 - hrast
    biljka hrast;
    hrast.ime_biljke="Hrast";
    hrast.opseg_c=boxPlot(cirkularnost_b4);
    hrast.opseg_k=boxPlot(konveksnost_b4);
    hrast.opseg_i=boxPlot(ispunjenost_b4);

    cout<<"Hrast opseg cirkularnosti: "<<hrast.opseg_c.at(0)<< " "<<hrast.opseg_c.at(1)<<std::endl;
    cout<<"Hrast opseg konveksnosti:  "<<hrast.opseg_k.at(0)<< " "<<hrast.opseg_k.at(1)<<std::endl;
    cout<<"Hrast opseg ispunjenosti:  "<<hrast.opseg_i.at(0)<< " "<<hrast.opseg_i.at(1)<<std::endl;


    //// Validacija
    vector<biljka> sve_biljke{grab,kalina,javor,hrast}; /* za koje imamo parametre */
    auto ulaz=data_set("validacija\\1",6,145 , false); /* racunanje parametara za validacijske slike */
    vector<Mat> slike_validacijske;
    Mat slika;

    for(int i = 1; i <= 6; i++){
         string path = "validacija\\1";
         path = path + "-" + to_string(i) + ".jpg";
         slika = imread(path);
         slike_validacijske.push_back(slika);
    }

    vector<string> ulaz_imena_biljki{"Grab","Grab","Kalina","Hrast","Hrast","Javor"};
    vector<string> izlaz;

    cout<<"Ulaz: "<<std::endl;
    for(auto x: ulaz_imena_biljki){
        cout<<x<<", ";
    }
    cout<<std::endl;

        izlaz=klasifikacija(sve_biljke,ulaz);

    cout<<"Izlaz: "<<std::endl;
    for(int i=0;i<izlaz.size();i++){
        auto x=izlaz.at(i);
        cout<<x<<", ";
    }
    cout<<std::endl;
    Point text_position(400, 400);
    int font_size = 10;//Declaring the font size//
    Scalar font_Color(0, 0, 0);//Declaring the color of the font//
    int font_weight = 2;

    for(int i = 0; i<slike_validacijske.size(); i++){
        putText(slike_validacijske.at(i), izlaz.at(i), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);
        imshow("Izlaz - slika "+to_string(i),slike_validacijske.at(i));
        imwrite("D:\\Vazni_Podaci_200gb\\Fakultet\\I Msc\\II\\UDOS\\Seminarski_izlaz\\"+to_string(i)+".jpg",slike_validacijske.at(i));


    }
    waitKey();

/// ZANEMARITI nastavak koda - koristeno za izvjestaj

    Mat primjer,maska;
    vector<vector<Point> > konture; /* Sadrzi sve konture sa binarne slike */
    vector<Point> kontura_lista; /* Sadrzi samo konturu lista */

    Mat element = getStructuringElement( MORPH_CROSS,
                                  Size( 4, 4 ),
                                  Point( -1, -1 ) );
    primjer=imread("primjer.png",IMREAD_COLOR);

    cvtColor(primjer, primjer, COLOR_BGR2GRAY);
        imshow("Gray scale",primjer);

    threshold(primjer,maska,230,255,THRESH_BINARY);
        imshow("Binarna slika",maska);

    erode(maska, maska, element);
    erode(maska, maska, element);
    erode(maska, maska, element);
    dilate(maska, maska, element);
    dilate(maska, maska, element);
    threshold(primjer,maska,230,255,THRESH_BINARY_INV);

         imshow("Binarna slika nakon morfoloskih operacija",  maska);

    vector<Vec4i> hijearhija;

    findContours( maska, konture, hijearhija, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE );
    Mat slika1(Size(primjer.cols,primjer.rows), CV_8UC3, Scalar(255, 255, 255));
    int n;
        for(size_t i=0;i<konture.size();i++){

            /* Najveca kontura je kontura lista, ostale zanemariti */
            if(konture.at(i).size()>500){
                kontura_lista=konture.at(i);
                n=i;
                Scalar color1 = Scalar(0, 0, 0 );
                /* Iscrtavanje samo konture lista */
                drawContours( slika1, konture, (int)i, color1 ,1);
                    imshow("Kontura lista", slika1);
               }
        }

    Point centar=centroid(kontura_lista);
    // centroid
    putText(slika1, "X", centar,FONT_HERSHEY_PLAIN, 2.1,Scalar(0, 0, 255), 2, LINE_AA);
    Mat slika2=slika1;
    double obim_lista = arcLength( kontura_lista, false);
    double r=obim_lista/(2*PI);
     // kruznica
    circle(slika1, centar, r , Scalar(0, 0, 0), 1);
    imshow("Kruznica sa istim obimom kao kontura lista",slika1);


    Scalar color1 = Scalar(0, 0, 0 );
    vector<vector<Point>> konveksni_oblik(1);
    convexHull( konture[n], konveksni_oblik[0] );

    //size_t i=0;
    drawContours( slika2, konture, (int)n, color1 ,1);

     drawContours( slika2, konveksni_oblik, (int)0, color1 ,1);

    imshow("Konveksna kontura",slika2);

    waitKey();



    return 0;
}
