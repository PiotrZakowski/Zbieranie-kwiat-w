#include <iostream>
using namespace std;
#define NIC -1
#define POZIOM_KORZENIA 1

//STRUKTURY DLA DRZEWA/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Tablica tych struktury stworzy drzewo.

struct wezel
{
	int id_wezla; // Pole to pamiêta id nadanemu wezlu; np. komenda 1 7 5 stworzy korzen drzewa, którego id bêdzie wynosi³o 7.
	int wsp_x; //   wsp_x i wsp_y to id_wezlow na koñcach danej œcie¿ki; np. komenda 3 8 15 sprawi ¿e wsp_x=3 wsp_y=8;
	int wsp_y; //wspó³rzêdne te pos³u¿a póŸniej do wyznaczania relacji ociec<->syn.
	int wartosc_wezla; //   Pole to pamiêta liczbe kwiatów jak¹ mo¿na zebraæ id¹c do tego wêz³a; jest to poniek¹d wartoœæ wêz³a.
	int poziom;
	int indeks_rodzic; //   Indeks (nie id_wezla rodzica!) rodzica wêz³a w tablicy, w której jest zaimplementowane drzewo.
	int indeks_lewy_syn; //   Indeks (nie id_wezla lewego syna!) lewego syna wêz³a w tablicy, w której jest zaimplementowane drzewo.
	int indeks_prawy_syn; //   Indeks (nie id_wezla prawego syna!) prawego syna wêz³a w tablicy, w której jest zaimplementowane drzewo.
	int liczba_wezlow_poddrzewa; //   Pole ktorego wartoœæ to liczba wêz³ów poddrzewa gdyby dany wêze³ by³ korzeniem; pole to pos³u¿y do
								 //optymalizacji kodu ( np. nie ma sensu dla wêz³a którego poddrzewo ma 3 wêz³y, dawaæ 5 mo¿liwych ruchów ).
	int* notatki; //   Pole to bedzie tablic¹ która zapamiêta w notatki[liczba_ruchow] raz wyliczone maksymalne œcie¿ki z danego wêz³a 
				  //przy u¿yciu x ruchów.
};

//FUNKCJE BUDOWANIA DRZEWA/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Funkcje przygotowuj¹ce tablice do zapisania w nim drzewa (ustawienia_poczatkowe), buduj¹ce drzewo (zbuduj_sciezki, stworz_powiazania)
//a tak¿e dbaj¹ce o konkretne pola wêz³ów(inkrementuj_liczbe_wezlow_poddrzewa, stworz_notatki).

//   Funkcja ta w ka¿dym wêŸle inicjuje indeks lewego i prawego syna na wartoœæ NIC (czyli -1), 
//gdy¿ póŸniej pozwoli to rozstrzygn¹æ czy slot na lewego syna jest ju¿ zajêty i trzeba zapisaæ pod slotem prawego syna.
void ustawienia_poczatkowe(wezel* drzewo, int liczba_wezlow)
{
	for (int i = 0; i < liczba_wezlow; i++)
	{
		drzewo[i].indeks_lewy_syn = NIC;
		drzewo[i].indeks_prawy_syn = NIC;
	}
}

//Funkcja rekurencyjna która idzie w górê drzewa od rodzica a¿ do korzenia, inkrementujac liczbê wêz³ów poddrzewa.
void inkrementuj_liczbe_wezlow_poddrzewa(wezel* drzewo,int indeks)
{
	drzewo[indeks].liczba_wezlow_poddrzewa += 1;
	if (drzewo[indeks].poziom != POZIOM_KORZENIA) inkrementuj_liczbe_wezlow_poddrzewa(drzewo, drzewo[indeks].indeks_rodzic);
}

//   Funkcja rekurencyjna, która najpierw wyszukuje korzeñ drzewa, a nastêpnie bada wêz³y wchodz¹ce z nim w relacje, 
//a nastêpnie wêz³y wchodz¹ce w relacji z poprzednimi wêz³ami itd. a¿ osi¹gniête zostan¹ liœcie. 
void stworz_powiazania(wezel* drzewo, int indeks, int liczba_wezlow)
{
	for (int i = 0; i < liczba_wezlow; i++)
	{
		if ((drzewo[i].wsp_x == drzewo[indeks].id_wezla || drzewo[i].wsp_y == drzewo[indeks].id_wezla) 
			&& indeks!=i) //indeks!=i zabezpiecza przed wybraniem raz jeszcze tego samego wêz³a.
		{
			//Zapisujemy w rodzicu indeks tablicy (nie id_wêz³a!) syna.
			if (drzewo[indeks].indeks_lewy_syn != NIC) drzewo[indeks].indeks_prawy_syn = i;
			else drzewo[indeks].indeks_lewy_syn = i;
			//Zapisujemy w wezle indeks rodzica (nie id_wêz³a!) i inkrementujemy liczbe wezlow poddrzewa wez³ów "nad" obecnym wêz³em.
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

//   Funkcja ta pobiera z wejscia trójki liczb reprezentuj¹ce wêze³, zapisuje je do tablicy, oraz dodatkowo wyznacza oznacza 
//wêze³ bêd¹cy korzeniem drzewa.
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
			//   Nie musimy zapisywaæ wsp_x i wsp_y korzenia, gdy¿ one pos³u¿¹ jedynie w przysz³oœci do wyznaczania relacji,
			//a relacja tego wêz³a jest prosta - jest to korzeñ drzewa.
		}
		else
		{
			//Jedyne co wiemy to id_wezlow na krancach sciezki.
			drzewo[i].wsp_x = koniec_x;
			drzewo[i].wsp_y = koniec_y;
		}
		//Inicjalizujemy liczbe wezlow poddrzewa (na pewno liczba ta jest >1 gdy¿ sam wêze³ tworzy poddrzewo o 1 wêŸle).
		drzewo[i].liczba_wezlow_poddrzewa = 1;
		//Zapisujemy wartosc korzenia/wezla.
		drzewo[i].wartosc_wezla = wartosc;
	}
	stworz_powiazania(drzewo, indeks_korzenia, liczba_wezlow);
}

//   Funkcja, która na podstawie tego ile wez³ów ma poddrzewo tworzy zainicjalizowan¹ wartoœci¹ NIC (czyli -1) tablicê, w której mo¿e 
//zapisywaæ wartoœci jakie zwraca funkcja wariacje_wezla podczas odnajdywania odpowiedniej œcie¿ki.
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

///ALGORYTM PRZECZESUJ¥CY//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//   Funkcja rekurencyjnie i iteracyjnie szuka œcie¿ki o najwiêkszej sumie wartoœci wêz³ów tej œcie¿ki (której d³ugoœæ tej œcie¿ki jest 
//narzucona odgórnie).
int wariacje_wezla(wezel* drzewo, int indeks, int liczba_do_zerwania)
{
	if (drzewo[indeks].indeks_lewy_syn == NIC && drzewo[indeks].indeks_prawy_syn == NIC) //Gdy jest to liœæ.
	{
		if (liczba_do_zerwania>0) return drzewo[indeks].wartosc_wezla;
		else return 0;
	}
	else
	{
		int maks = 0;
		if (drzewo[indeks].indeks_lewy_syn == NIC || drzewo[indeks].indeks_prawy_syn == NIC) 
			//   Gdy wêze³ ma jednego syna to nie ma iteracji - wszystkie mo¿liwe_œcie¿ki-1 idzie na tego jedynego syna.
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
				//   Je¿eli nigdy wczeœniej w tym wêŸle nie by³a liczona maksymalna liczba kwiatów dla takiej liczbie mozliwych ruchów;
				//jest to optymalizacja kodu nr 1.
			{
				for (int i = 0; i <= liczba_do_zerwania; i++)
				{
					if (i > drzewo[drzewo[indeks].indeks_lewy_syn].liczba_wezlow_poddrzewa
						|| (liczba_do_zerwania - i) > drzewo[drzewo[indeks].indeks_prawy_syn].liczba_wezlow_poddrzewa) continue;
					//   Opuszczanie kolejki, gdy do któregoœ syna wêdruje wiêcej mo¿liwych do wykoanania ruchów, ni¿ to potrzebne;
					//jest to optymalizacja kodu nr 2.
					liczba_z_lewego = wariacje_wezla(drzewo, drzewo[indeks].indeks_lewy_syn, i);
					liczba_z_prawego = wariacje_wezla(drzewo, drzewo[indeks].indeks_prawy_syn, liczba_do_zerwania - i);
					if (maks < liczba_z_lewego + liczba_z_prawego) maks = liczba_z_lewego + liczba_z_prawego;
				}
				drzewo[indeks].notatki[liczba_do_zerwania] = maks; //   Kiedy program raz wykona³ sumowanie, zapisuje sobie wynik tego 
				//sumowania by na przysz³oœæ nie sumowaæ kolejny raz; jest to optymalizacja nr 1.
			}
			else
			{
				maks = drzewo[indeks].notatki[liczba_do_zerwania]; //   Korzystanie z gotowego wyniku, raz wykonanego wczeœniej sumowania.
			}
			return drzewo[indeks].wartosc_wezla + maks;
		}
	}
}

//  Funkcja ma za zadanie jedynie odnaleŸæ w tablicy korzeñ, wywo³aæ dla niego funkcjê wariacje_wezla i ostatecznie wypisaæ wynik.
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