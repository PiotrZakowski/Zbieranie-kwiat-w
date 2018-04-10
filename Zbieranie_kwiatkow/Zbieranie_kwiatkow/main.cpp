#include <iostream>
using namespace std;
#define NIC -1
#define POZIOM_KORZENIA 1

//STRUKTURY DLA DRZEWA/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Tablica tych struktury stworzy drzewo.

struct wezel
{
	int id_wezla; // Pole to pami�ta id nadanemu wezlu; np. komenda 1 7 5 stworzy korzen drzewa, kt�rego id b�dzie wynosi�o 7.
	int wsp_x; //   wsp_x i wsp_y to id_wezlow na ko�cach danej �cie�ki; np. komenda 3 8 15 sprawi �e wsp_x=3 wsp_y=8;
	int wsp_y; //wsp�rz�dne te pos�u�a p�niej do wyznaczania relacji ociec<->syn.
	int wartosc_wezla; //   Pole to pami�ta liczbe kwiat�w jak� mo�na zebra� id�c do tego w�z�a; jest to poniek�d warto�� w�z�a.
	int poziom;
	int indeks_rodzic; //   Indeks (nie id_wezla rodzica!) rodzica w�z�a w tablicy, w kt�rej jest zaimplementowane drzewo.
	int indeks_lewy_syn; //   Indeks (nie id_wezla lewego syna!) lewego syna w�z�a w tablicy, w kt�rej jest zaimplementowane drzewo.
	int indeks_prawy_syn; //   Indeks (nie id_wezla prawego syna!) prawego syna w�z�a w tablicy, w kt�rej jest zaimplementowane drzewo.
	int liczba_wezlow_poddrzewa; //   Pole ktorego warto�� to liczba w�z��w poddrzewa gdyby dany w�ze� by� korzeniem; pole to pos�u�y do
								 //optymalizacji kodu ( np. nie ma sensu dla w�z�a kt�rego poddrzewo ma 3 w�z�y, dawa� 5 mo�liwych ruch�w ).
	int* notatki; //   Pole to bedzie tablic� kt�ra zapami�ta w notatki[liczba_ruchow] raz wyliczone maksymalne �cie�ki z danego w�z�a 
				  //przy u�yciu x ruch�w.
};

//FUNKCJE BUDOWANIA DRZEWA/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Funkcje przygotowuj�ce tablice do zapisania w nim drzewa (ustawienia_poczatkowe), buduj�ce drzewo (zbuduj_sciezki, stworz_powiazania)
//a tak�e dbaj�ce o konkretne pola w�z��w(inkrementuj_liczbe_wezlow_poddrzewa, stworz_notatki).

//   Funkcja ta w ka�dym w�le inicjuje indeks lewego i prawego syna na warto�� NIC (czyli -1), 
//gdy� p�niej pozwoli to rozstrzygn�� czy slot na lewego syna jest ju� zaj�ty i trzeba zapisa� pod slotem prawego syna.
void ustawienia_poczatkowe(wezel* drzewo, int liczba_wezlow)
{
	for (int i = 0; i < liczba_wezlow; i++)
	{
		drzewo[i].indeks_lewy_syn = NIC;
		drzewo[i].indeks_prawy_syn = NIC;
	}
}

//Funkcja rekurencyjna kt�ra idzie w g�r� drzewa od rodzica a� do korzenia, inkrementujac liczb� w�z��w poddrzewa.
void inkrementuj_liczbe_wezlow_poddrzewa(wezel* drzewo,int indeks)
{
	drzewo[indeks].liczba_wezlow_poddrzewa += 1;
	if (drzewo[indeks].poziom != POZIOM_KORZENIA) inkrementuj_liczbe_wezlow_poddrzewa(drzewo, drzewo[indeks].indeks_rodzic);
}

//   Funkcja rekurencyjna, kt�ra najpierw wyszukuje korze� drzewa, a nast�pnie bada w�z�y wchodz�ce z nim w relacje, 
//a nast�pnie w�z�y wchodz�ce w relacji z poprzednimi w�z�ami itd. a� osi�gni�te zostan� li�cie. 
void stworz_powiazania(wezel* drzewo, int indeks, int liczba_wezlow)
{
	for (int i = 0; i < liczba_wezlow; i++)
	{
		if ((drzewo[i].wsp_x == drzewo[indeks].id_wezla || drzewo[i].wsp_y == drzewo[indeks].id_wezla) 
			&& indeks!=i) //indeks!=i zabezpiecza przed wybraniem raz jeszcze tego samego w�z�a.
		{
			//Zapisujemy w rodzicu indeks tablicy (nie id_w�z�a!) syna.
			if (drzewo[indeks].indeks_lewy_syn != NIC) drzewo[indeks].indeks_prawy_syn = i;
			else drzewo[indeks].indeks_lewy_syn = i;
			//Zapisujemy w wezle indeks rodzica (nie id_w�z�a!) i inkrementujemy liczbe wezlow poddrzewa wez��w "nad" obecnym w�z�em.
			drzewo[i].indeks_rodzic = indeks;
			inkrementuj_liczbe_wezlow_poddrzewa(drzewo, indeks);
			//Zapisujemy poziom wezla o jeden wiekszy niz poziom rodzica.
			drzewo[i].poziom = drzewo[drzewo[i].indeks_rodzic].poziom + 1;
			if (drzewo[i].wsp_x == drzewo[indeks].id_wezla)
			{
				drzewo[i].id_wezla = drzewo[i].wsp_y;
				stworz_powiazania(drzewo, i, liczba_wezlow);
			}
			else
			{
				drzewo[i].id_wezla = drzewo[i].wsp_x;
				stworz_powiazania(drzewo, i, liczba_wezlow);
			}
		}
	}
}

//   Funkcja ta pobiera z wejscia tr�jki liczb reprezentuj�ce w�ze�, zapisuje je do tablicy, oraz dodatkowo wyznacza oznacza 
//w�ze� b�d�cy korzeniem drzewa.
void zbuduj_sciezki(wezel* drzewo, int liczba_wezlow)
{
	int koniec_x=0, koniec_y=0, wartosc=0;
	int indeks_korzenia;
	for (int i = 0; i < liczba_wezlow; i++)
	{
		scanf("%d %d %d", &koniec_x, &koniec_y, &wartosc);
		if (koniec_x == 1 || koniec_y == 1) //Pierwsze rozwidlenie - korzen.
		{
			indeks_korzenia = i;
			//Zapisujemy id dla korzenia.
			if (koniec_x == 1) drzewo[i].id_wezla = koniec_y;
			else drzewo[i].id_wezla = koniec_x;
			//Zapisujemy poziom korzenia ( czyli 1 ).
			drzewo[i].poziom = POZIOM_KORZENIA;
			//Korzen nie ma rodzica wiec nie zapisujemy w korzeniu indeksu rodzica.
			drzewo[i].indeks_rodzic = NIC;
			//   Nie musimy zapisywa� wsp_x i wsp_y korzenia, gdy� one pos�u�� jedynie w przysz�o�ci do wyznaczania relacji,
			//a relacja tego w�z�a jest prosta - jest to korze� drzewa.
		}
		else
		{
			//Jedyne co wiemy to id_wezlow na krancach sciezki.
			drzewo[i].wsp_x = koniec_x;
			drzewo[i].wsp_y = koniec_y;
		}
		//Inicjalizujemy liczbe wezlow poddrzewa (na pewno liczba ta jest >1 gdy� sam w�ze� tworzy poddrzewo o 1 w�le).
		drzewo[i].liczba_wezlow_poddrzewa = 1;
		//Zapisujemy wartosc korzenia/wezla.
		drzewo[i].wartosc_wezla = wartosc;
	}
	stworz_powiazania(drzewo, indeks_korzenia, liczba_wezlow);
}

//   Funkcja, kt�ra na podstawie tego ile wez��w ma poddrzewo tworzy zainicjalizowan� warto�ci� NIC (czyli -1) tablic�, w kt�rej mo�e 
//zapisywa� warto�ci jakie zwraca funkcja wariacje_wezla podczas odnajdywania odpowiedniej �cie�ki.
void stworz_notatki(wezel* drzewo, int liczba_wezlow)
{
	for (int i = 0; i < liczba_wezlow; i++)
	{
		drzewo[i].notatki = new int[drzewo[i].liczba_wezlow_poddrzewa];
		for (int j = 0; j < drzewo[i].liczba_wezlow_poddrzewa; j++)
		{
			drzewo[i].notatki[j] = NIC;
		}
	}
}

///ALGORYTM PRZECZESUJ�CY//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//   Funkcja rekurencyjnie i iteracyjnie szuka �cie�ki o najwi�kszej sumie warto�ci w�z��w tej �cie�ki (kt�rej d�ugo�� tej �cie�ki jest 
//narzucona odg�rnie).
int wariacje_wezla(wezel* drzewo, int indeks, int liczba_do_zerwania)
{
	if (drzewo[indeks].indeks_lewy_syn == NIC && drzewo[indeks].indeks_prawy_syn == NIC) //Gdy jest to li��.
	{
		if (liczba_do_zerwania>0) return drzewo[indeks].wartosc_wezla;
		else return 0;
	}
	else
	{
		int maks = 0;
		if (drzewo[indeks].indeks_lewy_syn == NIC || drzewo[indeks].indeks_prawy_syn == NIC) 
			//   Gdy w�ze� ma jednego syna to nie ma iteracji - wszystkie mo�liwe_�cie�ki-1 idzie na tego jedynego syna.
		{
			if (drzewo[indeks].indeks_lewy_syn == NIC) maks = wariacje_wezla(drzewo, drzewo[indeks].indeks_prawy_syn, liczba_do_zerwania - 1);
			else maks = wariacje_wezla(drzewo, drzewo[indeks].indeks_lewy_syn, liczba_do_zerwania - 1);
			return drzewo[indeks].wartosc_wezla + maks;
		}
		else
		{
			if (liczba_do_zerwania == 0) return 0;
			if (liczba_do_zerwania == 1) return drzewo[indeks].wartosc_wezla;
			liczba_do_zerwania--;
			int liczba_z_lewego;
			int liczba_z_prawego;
			int maks = 0;
			if (drzewo[indeks].notatki[liczba_do_zerwania] == NIC) 
				//   Je�eli nigdy wcze�niej w tym w�le nie by�a liczona maksymalna liczba kwiat�w dla takiej liczbie mozliwych ruch�w;
				//jest to optymalizacja kodu nr 1.
			{
				for (int i = 0; i <= liczba_do_zerwania; i++)
				{
					if (i > drzewo[drzewo[indeks].indeks_lewy_syn].liczba_wezlow_poddrzewa
						|| (liczba_do_zerwania - i) > drzewo[drzewo[indeks].indeks_prawy_syn].liczba_wezlow_poddrzewa) continue;
					//   Opuszczanie kolejki, gdy do kt�rego� syna w�druje wi�cej mo�liwych do wykoanania ruch�w, ni� to potrzebne;
					//jest to optymalizacja kodu nr 2.
					liczba_z_lewego = wariacje_wezla(drzewo, drzewo[indeks].indeks_lewy_syn, i);
					liczba_z_prawego = wariacje_wezla(drzewo, drzewo[indeks].indeks_prawy_syn, liczba_do_zerwania - i);
					if (maks < liczba_z_lewego + liczba_z_prawego) maks = liczba_z_lewego + liczba_z_prawego;
				}
				drzewo[indeks].notatki[liczba_do_zerwania] = maks; //   Kiedy program raz wykona� sumowanie, zapisuje sobie wynik tego 
				//sumowania by na przysz�o�� nie sumowa� kolejny raz; jest to optymalizacja nr 1.
			}
			else
			{
				maks = drzewo[indeks].notatki[liczba_do_zerwania]; //   Korzystanie z gotowego wyniku, raz wykonanego wcze�niej sumowania.
			}
			return drzewo[indeks].wartosc_wezla + maks;
		}
	}
}

//  Funkcja ma za zadanie jedynie odnale�� w tablicy korze�, wywo�a� dla niego funkcj� wariacje_wezla i ostatecznie wypisa� wynik.
void znajdz_odpowiednia_sciezke(wezel* drzewo, const int liczba_do_zerwania, const int liczba_sciezek)
{
	int maks=0;
	for (int i = 0; i < liczba_sciezek; i++)
	{
		if (drzewo[i].poziom == POZIOM_KORZENIA)
		{
			maks=wariacje_wezla(drzewo, i, liczba_do_zerwania);
			break;
		}
	}
	printf("%d", maks);
}

////////////////////////////////////////////////////////////////////MAIN///////////////////////////////////////////////////////////////////

int main()
{
	wezel* drzewo;
	int liczba_sciezek = 0, liczba_do_zerwania = 0, maks=0;
	scanf("%d %d", &liczba_sciezek, &liczba_do_zerwania);
	drzewo = new wezel[liczba_sciezek];
	ustawienia_poczatkowe(drzewo, liczba_sciezek);
	zbuduj_sciezki(drzewo, liczba_sciezek);
	stworz_notatki(drzewo, liczba_sciezek);
	znajdz_odpowiednia_sciezke(drzewo, liczba_do_zerwania, liczba_sciezek);
}