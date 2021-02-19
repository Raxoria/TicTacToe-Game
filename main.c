/*********************************************************************
 * Tic-Tac-Toe Game  
 * ===================================================================
 * NAMA FILE    : main.c
 * AUTHOR       : Ihsan Fauzan Hanif & Syelvie Ira Ratna Mulia
 * COMPILER     : MinGW GCC
 * DESKRIPSI    : Aplikasi konsol permainan Tic-Tac-Toe
 * 2021
 ********************************************************************/

/**
 * Header File
 * ===================================================================
 * File yang mengandung modul dan makro di mana dapat secara langsung
 * digunakan tanpa perlu dibuat secara manual.
 */
#include <conio.h>      // Console input output
#include <ctype.h>      // C-type
#include <direct.h>     // Direktori
#include <errno.h>      // Error condition
#include <pthread.h>    // Threading
#include <stdbool.h>    // Standard boolean
#include <stdio.h>      // Standard input output
#include <stdlib.h>     // Standard library
#include <string.h>     // String
#include <sys/stat.h>   // File info
#include <time.h>       // Time'h
#include <unistd.h>     // Unix standard
#include <windows.h>    // Windows API

/**
 * Makro Preprocessor
 * ===================================================================
 * Bagian kode program yang diberi nama di mana kemudian ditransformasi
 * menjadi konten yang dimaksud oleh makro ketika proses kompilasi
 */

//#define DEBUG                 // Mode kompilasi
#define GAME_TIMER 10           // Waktu setiap giliran (satuan detik)
#define KEYBOARD_ENTER 13       // Nomor ASCII untuk Enter Key
#define KEYBOARD_ESC 27         // Nomor ASCII untuk Escape Key
#define MAX_SAVEGAME 5          // Batasan data permainan yang bisa disimpan
#define SIMBOL_P1 'X'           // Simbol pemain pertama
#define SIMBOL_P2 'O'           // Simbol pemain kedua

/**
 * Awal perhitungan minimax pada papan dengan ukuran terkait berdasarkan
 * jumlah petak yang dapat diisi
 */ 
#define START_MINIMAX_5x5 22 
#define START_MINIMAX_7x7 46

#ifdef DEBUG
    // Makro yang tersedia apabila DEBUG terdefinisi.
    #define PATH_CARA "D:/Arsip Kuliah/Coding/TicTacToe-Game/debug/CaraBermain.txt"
    #define PATH_SKOR "D:/Arsip Kuliah/Coding/TicTacToe-Game/debug/skor"
    #define PATH_GAME "D:/Arsip Kuliah/Coding/TicTacToe-Game/debug/game/save"
#else
    // Makro yang tersedia apabila DEBUG tidak terdefinisi.
    #define PATH_GAME "game/save"
    #define PATH_CARA "CaraBermain.txt"
    #define PATH_SKOR "skor"
#endif

/**
 * Enumerasi
 * ===================================================================
 * Nama yang merepresentasikan nilai tertentu yang sesuai dengan apa
 * yang telah didefinisikan berikut
 */

// Nilai waktu yang digunakan untuk mendeterminasi hasil input
typedef enum
{
    TERLAMBAT = -1,
    KELUAR = -2,
    SELESAI = -3
} InputPapan;

/**
 * Struktur
 * ===================================================================
 * Tipe data bentukan yang terdiri dari tipe data tertentu di mana
 * berfungsi untuk mengelompokkan data berdasarkan tujuan penggunaannya
 */

// Tipe Data Pemain
typedef struct
{
	bool isKomputer;    // TRUE apabila pemain adalah komputer
	char nama[50];      // Nama pemain
	char simbol;        // Simbol yang digunakan pada permainan
	int input;          // Input petak permainan
	long long int skor; // Skor pemain
} Pemain;

// Tipe Data Papan permainan
typedef struct 
{
    char nama[25];      // Label pada savegame untuk menyimpan identitas papan
	int ukuran;         // Ukuran papan (banyak petak secara horizontal & vertikal)
	int waktu;          // Waktu yang masih tersedia (satuan detik)
	int kesulitan;      // Tingkat kesulitan ketika melawan komputer
	int giliran;        // Giliran pemain, 0 = pemain ke-1, 1 = pemain ke-2
	char isi[10][10];   // Isi pada petak permainan
	Pemain pemain[2];   // Data para pemain
} Papan;

// Tipe Data Skor
typedef struct
{
    char nama[50];      // Nama pemain yang disimpan
    char kesulitan[10]; // Tingkat kesulitan yang dimainkan dalam bentuk kata
    int skor;           // Skor yang telah diperoleh pemain
} SkorInfo;

/**
 * Variable Global
 * ===================================================================
 * Variable yang dapat diakses di seluruh lingkup kode program dan
 * dapat terus digunakan oleh program hingga program dihentikan
 */
HANDLE hConsole;            // Console handler
WORD consoleAttr;           // Atribut console
pthread_t thTimer, thGame;  // Handle Thread/proses yang dieksekusi
bool inputValid = true;     // FALSE apabila input tidak sesuai dengan yang diminta
bool jeda = false;          // TRUE apabila permainan tengah dijeda
int idPapan = 0;            // 0 = papan baru, dan tertentu sesuai dengan id papan permainan 
                            // yang dipilih apabila melanjutkan permainan
Papan papan;                // Papan permainan
SkorInfo sInfo[10];         // Data dari daftar 10 pemain dengan skor tertinggi

/**
 * Prototipe Modul
 * ===================================================================
 * Deklarasi modul yang menginformasikan pada kompiler terkait dengan
 * fungsi yang nanti dapat digunakan pada program
 * 
 * Hal ini karena proses pembacaan fungsi bersifat mengalir dari atas ke
 * bawah di mana isi fungsi yang berada di atas tidak bisa membaca isi
 * fungsi dibawahnya sehingga perlu prototipe terlebih dahulu
 */
void bagianCara();
void bagianMain();
void bagianKeluar();
void bagianSkor();
bool cekDiagonal(char);
bool cekHorizontal(char);
bool cekVertikal(char);
bool cekPapan(char);
void clearConsole();
int deletePermainan(int);
int deleteSkor();
int eval();
int getJangkauan(int, int);
char* getKesulitanStr(int);
void getInput(char*);
int getInputKomputer();
char getIsi(int);
int getTotalKosong();
int imporPermainan(int, Papan*);
int imporSkor();
bool isBisaDiisi();
int isiAcak();
int isiKritis();
void initComponent();
void initDirectory();
void jedaPermainan();
void menuGameOver(int);
void menuHapusPermainan();
void menuHapusSkor();
void menuKalah(int);
void menuMenang(int);
void menuMultipemain(int);
void menuSeri(int);
void menuSimpan();
void menuSimpanSiap(int);
void menuSimpanSelesai();
void menuUtama();
int minimax(bool, int, int, int, int);
void mulaiPermainan();
int panjangAngka(int);
int parsePosisi(int, int);
void pesanInvalid(char*);
void reverse(char*, int);
int saveSkor();
int savePermainan(int);
void saveCaraMain(char*);
void setKomputer();
void setMultipemain();
void setModePermainan();
void setPermainan();
void setPermainanLama();
void setIsi(int, char);
void setUkuran();
void showPermainanLama();
void showSkor();
void showPapan(bool);
void sortSkor();
void *threadPermainan();
void *threadTimer();
void upperCase(char*);

/**
 * Entry Point
 * ===================================================================
 * Bagian program yang dieksekusi pada awal program ketika dijalankan
 */
int main(int argc, char *argv[])
{
    initComponent();
	menuUtama();
	// Indikasi dari program dapat dieksekusi secara sukses
    return 0;
}

/**
 * Implementasi Modul
 * ===================================================================
 * Pernyataan atau isi dari modul yang memberikan pernyataan terkait
 * dengan apa yang dilakukan oleh modul tersebut
 */

/**
 * Inisialisasi dari komponen program yang dilakukan sebelum memulai
 * permainan, seperti mengaitkan informasi mengenai console terhadap
 * handler dan pengaturan agar variable memiliki nilai awal nol.
 */
void initComponent()
{
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    // Pembuat nol terhadap nilai dari variable
    memset(&papan, 0, sizeof(papan));
    memset(&sInfo, 0, sizeof(sInfo));
    inputValid = true;
    jeda = false;
    idPapan = 0;
    // Mode pembatalan sistem threading
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    // Memberikan judul pada konsol
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTitle("Tic-Tac-Toe Game");
    consoleAttr = consoleInfo.wAttributes;
    // pemanggilan fungsi untuk inisialisasi directory
    initDirectory();
}

/**
 * Inisialisasi folder yang diperlukan oleh program di mana
 * di mana program dijalankan
 */
void initDirectory()
{
    char cwd[1000] = {'\000'};  // Nama lokasi penyimpanan
    errno = 0;                  // Nilai error yang diberikan
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        // Apabila lokasi valid maka cari folder //game
        saveCaraMain(cwd);
        memcpy(cwd + strlen(cwd), "\\game", 5);
        int status = mkdir(cwd);
        if (status == -1 && errno != EEXIST)
        {
            // Apabila terdapat kendala dan folder tidak tersedia, maka tampilkan
            // perintah untuk melakukan operasi manual kepada pengguna.
            printf("Harap untuk membuat folder 'game' terlebih dahulu");
            printf("\nError Code: %d\n", errno);
            system("pause");
        }
    }
}

/**
 * Inisialisasi terkait dengan cara bermain pada file "CaraBermain.txt"
 */
void saveCaraMain(char *path)
{
    int lenPathCara = strlen(path) + 20;
    // Alokasi memori untuk menyimpan nama lokasi penyimpanan
    char *pathCara = malloc(lenPathCara);
    memset(pathCara, 0, lenPathCara);
    memcpy(pathCara + strlen(pathCara), "CaraBermain.txt", strlen("CaraBermain.txt"));
    FILE *in = fopen(pathCara, "r");
    if (in) fclose(in); // Apabila file sudah ada, maka tutup file
    else
    {
        // Apabila tidak ada, maka buat file "CaraBermain.txt"
        // bersamaan dengan kontennya
        fclose(in);
        in = fopen(pathCara, "w");
        fprintf(in, "======================================================");
        fprintf(in, "\nCara Bermain");
        fprintf(in, "\n======================================================");
        fprintf(in, "\n");
        fprintf(in, "\n[1.] Pilih \"Mulai Permainan\" pada Menu Utama");
        fprintf(in, "\n[2.] Pilih \"Permainan Baru\" atau \"Lanjutkan\""); 
        fprintf(in, "\n[3.] Pilih Ukuran Papan Permainan (Apabila memilih \"Permainan Baru\");");
        fprintf(in, "\n[4.] Pilih Mode Permainan");
        fprintf(in, "\nKeterangan:");
        fprintf(in, "\n  a. Tingkat Kesulitan Komputer yaitu mudah, sedang, dan sulit");
        fprintf(in, "\n  b. Masukan nama user jika memilih Multipemain");
        fprintf(in, "\n[5.] Masukkan angka pada petak yang masih kosong");
        fprintf(in, "\n[6.] User diberi batasan waktu dalam menentukan selama 10 detik");
        fprintf(in, "\n[7.] Permainan Berakhir");
        fprintf(in, "\nKeterangan:");
        fprintf(in, "\n  a. 3 petak terisi untuk 3x3 (horizontal, vertikal, diagonal)");
        fprintf(in, "\n  b. 4 petak terisi untuk 4x4 (horizontal, vertikal, diagonal)"); 
        fprintf(in, "\n  c. 5 petak terisi untuk 7x7 (horizontal, vertikal, diagonal)");
        fprintf(in, "\n[8.] Skor permainan dapat disimpan dan permainan bisa dilanjut di lain waktu");
        fprintf(in, "\n[9.] Pemain bisa memilih \"Ulang Permainan\" atau \"Keluar\" pada akhir permainan");
        fprintf(in, "\n[10.] Pemain bisa memilih tingkat kesulitan yang baru pada menu utama");
        fclose(in);
    }
    // Dealokasikan memori yang dipesan
    free(pathCara);
}

/**
 * Menghapus screen pada konsol dari output yang ditampilkan
 */
void clearConsole()
{
    system("cls");
}

/**
 * Mendapatkan input bertipe char dan mengubahnya pada bentuk uppercase
 * @param input alamat yang digunakan untuk menyimpan hasil input
 */
void getInput(char *input)
{
    char temp = '\000';
    fflush(stdin);  // menghapus data buffer terlebih dahulu
    scanf(" %c", &temp);
    *input = toupper(temp);
}

/**
 * Menampilkan pesan apabila input tidak valid
 * @param pesan kalimat pesan yang ditampilkan
 */
void pesanInvalid(char *pesan)
{
    if (!inputValid)
    {
        printf("%s", pesan);
        // Mengubahnya TRUE kembali karena penampilan pesan
        // sudah terlaksana
        inputValid = true;
    }
}

/**
 * Mendapatkan maksud dari nilai yang direpresentasikan oleh
 * tingkat kesulitan
 * @param kesulitan nilai tingkat kesulitan
 * @return kata yang dimaksud oleh tingkat kesulitan
 */
char* getKesulitanStr(int kesulitan)
{
    switch (kesulitan)
    {
    case 1: return "mudah";
    case 2: return "sedang";
    case 3: return "sulit";
    default: return "";
    }
}

/**
 * Tampilan menu utama pada program
 */
void menuUtama()
{
    char input = '\000';
    idPapan = 0;
    clearConsole();
    printf("======================================================");
    printf("\nMenu Utama");
    printf("\n======================================================");
    printf("\n");
	printf("\nBerikut merupakan pilihan yang dapat anda pilih.");
    printf("\n(Pilihlah opsi berikut dalam bentuk angka)");
	printf("\n[1.] Mulai Permainan");
	printf("\n[2.] Daftar 10 Pemain dengan Skor Tertinggi");
	printf("\n[3.] Cara Bermain");
	printf("\n[4.] Keluar Permainan");
    printf("\n");
	pesanInvalid("\nPilihan anda tidak valid");
	printf("\nMasukan pilihan: ");
    getInput(&input);
    
    int opsi = atoi(&input);
    switch (opsi)
    {
    case 1: // Opsi mulai permainan
        bagianMain();
        break;
    case 2: // Opsi daftar 10 pemain dengan skor tertinggi
        bagianSkor();
        break;
    case 3: // Opsi cara bermain
        bagianCara();
        break;
    case 4: // Opsi keluar permainan
        bagianKeluar();
        break;
    default:
        // Apabila opsi tidak valid, maka panggil kembali fungsi ini (rekursif)
        // dengan keadaan adanya pesan bahwa input tidak valid
		inputValid = false;
		menuUtama();
		break;
    }
}

/**
 * Konfigurasi jenis permainan yang akan dimainkan
 */
void bagianMain()
{
    char input = '\000';
    clearConsole();
    memset(&papan, 0, sizeof(papan));
    printf("======================================================");
    printf("\nMulai Permainan");
    printf("\n======================================================");
    printf("\n");
    printf("\nPilihlah jenis permainan yang akan anda mainkan");
    printf("\n[1.] Permainan Baru");
    printf("\n[2.] Lanjutkan Permainan Sebelumnya");
    printf("\nQ: Kembali");
    printf("\n");
	pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    if (input == 'Q') return menuUtama();

    int opsi = atoi(&input);
    switch (opsi)
    {
    case 1: // Opsi permainan baru
        setUkuran();
        setModePermainan();
        if (!papan.pemain[0].isKomputer && papan.pemain[1].isKomputer) 
        {
            // Set nama pemain ke-1 menjadi "Anda" dan pemain ke-2 menjadi "Komputer"
            memset(papan.pemain[0].nama, '\000', sizeof(papan.pemain[0].nama));
            memset(papan.pemain[1].nama, '\000', sizeof(papan.pemain[1].nama));
            memcpy(papan.pemain[0].nama, "Anda", 4);
            memcpy(papan.pemain[1].nama, "Komputer", 9);
            setKomputer();
        }
        else setMultipemain();
        break;
    case 2: // permainan lama
        setPermainanLama();
        break;
    default:
        inputValid = false;
        bagianMain();
        break;
    }
    setPermainan();
}

/**
 * Konfigurasi ukuran untuk permainan baru
 */
void setUkuran()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Baru");
    printf("\n======================================================");
    printf("\n");
    printf("\nPilihlah ukuran dari papan permainan");
    printf("\n[1.] 3 x 3");
    printf("\n[2.] 5 x 5");
    printf("\n[3.] 7 x 7");
    printf("\nQ: Kembali");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    if (input == 'Q') return bagianMain();
    
    int pilihanInt = atoi(&input);
    switch (pilihanInt)
    {
    case 1: // Ukuran 3 x 3
        papan.ukuran = 3;
        break;
    case 2: // Ukuran 5 x 5
        papan.ukuran = 5;
        break;
    case 3: // Ukuran 7 x 7
        papan.ukuran = 7;
        break;
    default:
        inputValid = false;
        setUkuran();
        break;
    }
}

/**
 * Konfigurasi mode permainan
 */
void setModePermainan()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Baru");
    printf("\n======================================================");
    printf("\n");
    printf("\nPilihlah mode permainan yang akan dimainkan");
    printf("\n[1.] Komputer");
    printf("\n[2.] Multipemain");
    printf("\nQ: Kembali");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    if (input == 'Q') return bagianMain();

    int pilihanInt = atoi(&input);
    switch (pilihanInt)
    {
    case 1: // Mode lawan komputer
        papan.pemain[0].isKomputer = false; // pemain ke-1 adalah pengguna
        papan.pemain[1].isKomputer = true;  // pemain ke-2 adalah mutlak komputer
        break;
    case 2: // Mode multipemain
        papan.pemain[0].isKomputer = false;
        papan.pemain[1].isKomputer = false;
        break;
    default:
        inputValid = false;
        setModePermainan();
        break;
    }
}

/**
 * Konfigurasi tingkat kesulitan dari komputer
 */
void setKomputer()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Baru");
    printf("\n======================================================");
    printf("\n");
	printf("\nPilihlah tingkat kesulitan");
	printf("\n[1.] Mudah");
	printf("\n[2.] Sedang");
	printf("\n[3.] Sulit");
    printf("\nQ: Kembali");
    printf("\n");
	pesanInvalid("\nPilihan anda tidak valid");
	printf("\nMasukan pilihan: ");
    getInput(&input);
    if (input == 'Q') return bagianMain();

    int pilihanInt = atoi(&input);
    switch (pilihanInt)
	{
	case 1: // Tingkat mudah
        papan.kesulitan = 1;
		break;
	case 2: // Tingkat sedang
        papan.kesulitan = 2;
		break;
	case 3: // Tingkat sulit
        papan.kesulitan = 3;
		break;
	default:
		inputValid = false;
		setKomputer();
		break;
	}
}

/**
 * Konfigurasi nama pemain pertama dan kedua untuk mode multipemain
 */
void setMultipemain()
{
    char input = '\000';
    char input1[25] = {'\000'};
    char input2[25] = {'\000'};
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Baru");
    printf("\n======================================================");
    printf("\n");
    pesanInvalid("\nNama harus memiliki (3-20) karakter");
	printf("\nHarap masukkan nama pemain");
	printf("\nNama pemain pertama: ");
    fflush(stdin);
    fgets(input1, 25, stdin);
    if (input1[strlen(input1) - 1] == '\n') input1[strlen(input1) - 1] = '\000';
    if (strlen(input1) < 3 || strlen(input1) >= 25)
    {
        inputValid = false;
        return setMultipemain();
    }
	printf("Nama pemain kedua: ");
    fflush(stdin);
	fgets(input2, 25, stdin);
    if (input2[strlen(input2) - 1] == '\n') input2[strlen(input2) - 1] = '\000';
    if (strlen(input2) < 3 || strlen(input2) >= 25)
    {
        inputValid = false;
        return setMultipemain();
    }
    memset(papan.pemain[0].nama, '\000', sizeof(papan.pemain[0].nama));
	memset(papan.pemain[1].nama, '\000', sizeof(papan.pemain[1].nama));
    memcpy(papan.pemain[0].nama, input1, strlen(input1));
    memcpy(papan.pemain[1].nama, input2, strlen(input2));

    showSet:
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Baru");
    printf("\n======================================================");
    printf("\n");
    printf("\nPastikan bahwa data ini sesuai");
    printf("\nNama pemain pertama : %s", papan.pemain[0].nama);
	printf("\nNama pemain kedua   : %s", papan.pemain[1].nama);
    printf("\nQ: Kembali | S: Sesuai");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    switch (input)
	{
	case 'Q': // kembali
		setMultipemain();
		break;
	case 'S': // mulai permainan
		break;
	default:
		inputValid = false;
		goto showSet;
		break;
	}
}

/**
 * Penghapusan seluruh data permainan lama
 */
void menuHapusPermainan()
{
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Lama");
    printf("\n======================================================");
    printf("\n");
    printf("\nApakah anda ingin menghapus semua data permainan?");
    printf("\nTekan 'Enter' untuk setuju atau 'Esc' untuk ke menu utama"); // pesan konfirmasi
    while (true)
    {
        // Lakukan loop sampai input dapat diproses
        fflush(stdin);
        char input = getch();
        // Apabila 'Enter', maka hapus data permainan
        if ((int)input == KEYBOARD_ENTER)
        {
            int i;
            for (i = 1; i <= MAX_SAVEGAME; i++)
            {
                int status = deletePermainan(i);
            }
            return bagianMain();
        }
        // Apabila 'Esc', maka kembali
        else if ((int)input == KEYBOARD_ESC) return setPermainanLama();
    }
}

/**
 * Konfigurasi data permainan lama
 */
void setPermainanLama()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nPermainan Lama");
    printf("\n======================================================");
    printf("\n");
    printf("\nPilihlah permainan yang ingin anda lanjutkan");
    showPermainanLama();
    printf("\nQ: Kembali | D: Hapus Semua");
    printf("\n");
    pesanInvalid("\nPilihan data permainan tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    if (input == 'Q') return bagianMain();  // Kembali
    else if (input == 'D') return menuHapusPermainan(); // Hapus semua data
    
    int pilihanInt = atoi(&input);
    if (pilihanInt > 0 && pilihanInt <= MAX_SAVEGAME)
    {
        // Apabila pengguna memilih melanjutkan, maka muat data
        // berdasarkan nilai (ID) yang dipilih
        if (imporPermainan(pilihanInt, &papan) == 0)
        {
            idPapan = pilihanInt;
            return;
        }
    }
    inputValid = false;
    return setPermainanLama();
}

/**
 * Menampilkan data permainan lama yang tersedia
 */
void showPermainanLama()
{
    int i = 0;
    for (i = 0; i < MAX_SAVEGAME; i++)
    {
        Papan temp;
        printf("\n[%d.] ", i + 1);
        if (imporPermainan(i + 1, &temp) == 0) printf("%-8.25s | %d x %d | Skor: %d", temp.nama, temp.ukuran, temp.ukuran, temp.pemain[0].skor);
        else printf("%-8.25s | %-5.4s | -", "Kosong", "-");
    }
}

/**
 * Impor data permainan lama
 * @param pos posisi (ID) data penyimpanan
 * @param pPapan variable untuk menampung hasil impor
 * @return 0 jika berhasil, 1 jika error
 */
int imporPermainan(int pos, Papan* pPapan)
{
    int pPapanSize = sizeof(*pPapan);
    char fileData[255] = {'\000'};
    char fileLokasi[255] = PATH_GAME;
    char* dataPapan = malloc(pPapanSize);
    memset(dataPapan, 0, pPapanSize);
    memset(pPapan, 0, pPapanSize);
    memcpy(fileData, fileLokasi, strlen(fileLokasi));
    itoa(pos, fileData + strlen(fileData), 10);
    memcpy(fileData + strlen(fileData), ".dat", 4);
    
    FILE* pFile = fopen(fileData, "rb");
    if (pFile == NULL) return 1;
    if (fread(dataPapan, pPapanSize, 1, pFile) != 1) return 1;
    reverse(dataPapan, pPapanSize);
    memcpy(pPapan, dataPapan, pPapanSize - 1);
    free(dataPapan);
    fclose(pFile);
    return 0;
}

/**
 * Simpan data permainan lama
 * @param pos posisi (ID) data penyimpanan
 * @return 0 jika berhasil, 1 jika error
 */
int savePermainan(int pos)
{
    int papanSize = sizeof(papan);
    char fileData[255] = {'\000'};
    char fileLokasi[255] = PATH_GAME;
    char *dataPapan = malloc(papanSize);
    memset(papan.pemain[0].nama, 0, sizeof(papan.pemain[0].nama));
    memset(papan.pemain[1].nama, 0, sizeof(papan.pemain[1].nama));
    memcpy(papan.pemain[0].nama, "Anda", 4);
    memcpy(papan.pemain[1].nama, "Komputer", 9);
    memcpy(fileData, fileLokasi, strlen(fileLokasi));
    itoa(pos, fileData + strlen(fileData), 10);
    memcpy(fileData + strlen(fileData), ".dat", 4);
    
    FILE* pFile = fopen(fileData, "wb");
    if (pFile == NULL) return 1;
    memset(dataPapan, 0, papanSize);
    memcpy(dataPapan, &papan, papanSize - 1);
    reverse(dataPapan, papanSize);
    if (fwrite(dataPapan, papanSize, 1, pFile) != 1) return 1;
    fclose(pFile);
    free(dataPapan);
    return 0;
}

/**
 * Hapus data permainan lama
 * @param pos posisi (ID) data penyimpanan
 * @return 0 jika berhasil, 1 jika error
 */
int deletePermainan(int pos)
{
    char fileData[255] = {'\000'};
    char fileLokasi[255] = PATH_GAME;
    memcpy(fileData, fileLokasi, strlen(fileLokasi));
    itoa(pos, fileData + strlen(fileData), 10);
    memcpy(fileData + strlen(fileData), ".dat", 4);
    return remove(fileData);
}

/**
 * Membalikkan nilai pada array. Contohnya nilai array [10][12] menjadi [12][10]
 * @param data nilai array yang dimaksudkan
 * @param length panjang dari nilai yang akan dibalik
 */
void reverse(char* data, int length)
{
    int i = 0;
    int j;
    int len = j = length - 1;
    while (i < j)
    {
        char temp = *(data + i);
        *(data + i) = *(data + len - i);
        *(data + len - i) = temp;
        i++;
        j--;
    }
}

/**
 * "Persiapan" sebelum memulai permainan
 */
void setPermainan()
{
    char input = '\000';
    char kesulitan[10] = {'\000'};
    memcpy(kesulitan, getKesulitanStr(papan.kesulitan), strlen(getKesulitanStr(papan.kesulitan)));
    clearConsole();
    printf("======================================================");
    printf("\nMulai Permainan");
    printf("\n======================================================");
    printf("\n");
    printf("\nBerikut merupakan data permainan yang akan dimulai.");
    printf("\nPemain 1          : %s", papan.pemain[0].nama);
    printf("\nPemain 2          : %s", papan.pemain[1].nama);
    printf("\nUkuran Papan      : %d x %d", papan.ukuran, papan.ukuran);
    if (papan.pemain[1].isKomputer)
    {
        printf("\nTingkat Kesulitan : %s", kesulitan);
        printf("\nSkor Saat ini     : %d", papan.pemain[0].skor);
    }
    else printf("\n[Mode Multipemain]");
    printf("\n");
    printf("\nTekan 'Enter' untuk memulai permainan atau 'Esc' untuk ke menu utama");

    while (true)
    {
        fflush(stdin);
        char input = getch();
        if ((int)input == KEYBOARD_ENTER) return mulaiPermainan();
        else if ((int)input == KEYBOARD_ESC) return bagianMain();
    }
}

/**
 * Pusat kendali ketika permainan dimulai
 */
void mulaiPermainan()
{
    bool sesuai = true;
    papan.giliran = 1;
    int giliran = papan.giliran;
    papan.pemain[0].simbol = SIMBOL_P1;
    papan.pemain[1].simbol = SIMBOL_P2;
    memset(papan.isi, '\000', sizeof(papan.isi));
    do
    {
        jeda = false;
        // Pengaturan waktu
        papan.waktu = GAME_TIMER;
        papan.giliran = (papan.giliran == 1)? 0 : 1;
        pthread_create(&thGame, NULL, threadPermainan, NULL);
        pthread_create(&thTimer, NULL, threadTimer, NULL);
        pthread_join(thGame, NULL);
        pthread_join(thTimer, NULL);

        int pos = 0;
        char simbol = papan.pemain[papan.giliran].simbol;
        switch (papan.waktu)
        {
        case TERLAMBAT:
            pos = isiAcak();
            break;
        case KELUAR:
            papan.waktu = 0;
            return bagianMain();
            break;
        case SELESAI:
            pos = papan.pemain[papan.giliran].input;
            break;
        }
        setIsi(pos, simbol);
        sesuai = cekPapan(simbol);
    }
    while (sesuai);
    // Apabila kondisi sudah dapat dinyatakan MENANG, KALAH, atau SERI
    // maka selanjutnya adalah memproses kondisi tersebut dan menampilkan
    // menu game over
    int status;
    if (papan.giliran == 2) status = 2;
    else if (!cekPapan(SIMBOL_P1) && papan.pemain[1].isKomputer == true) 
    {
        // Skor bertambah bagi pemain pertama apabila menang
        status = 0;
        papan.pemain[0].skor += 100;
    }
    else status = 1;
    menuGameOver(status);
}

/**
 * Pengaturan bagaimana timer bekerja
 */
void* threadTimer()
{
    // Diberikan toleransi waktu satu detik
    papan.waktu++;
    while (papan.waktu > -1)
    {
        if (jeda) continue;
        papan.waktu--;
        sleep(1); // Delay satu detik untuk setiap pengurangan waktu tersisa
    }
    return NULL;
}

/**
 * Operasi ketika permainan dilakukan
 */
void* threadPermainan()
{
    char input[5] = {'\000'};
    do
    {
        // Jika pemain memilih untuk keluar, maka hentikan loop
        if (papan.waktu == KELUAR) break;
        clearConsole();
        printf("======================================================");
        printf("\nPermainan %d x %d (%s vs %s)", papan.ukuran, papan.ukuran, papan.pemain[0].nama, papan.pemain[1].nama);
        printf("\n======================================================");
        printf("\n");
        // Jika permainan dijeda, maka panggil modul jeda permainan
        if (jeda) jedaPermainan();
        else
        {
            int posPapan = 0;
            showPapan(true);
            // Tampilkan skor apabila mode permainan melawan komputer
            if (papan.pemain[1].isKomputer) printf("\nSkor: %d", papan.pemain[0].skor);
            printf("\nQ: Ke menu utama");
            printf("\n");
            printf("\n[Giliran %s]", papan.pemain[papan.giliran].nama);
            pesanInvalid("\nPosisi petak tidak valid");
            // Apabila giliran komputer (papan.giliran = 1) perlu melakukan
            // untuk mendapatkan pilihan posisi dari komputer
            if (papan.pemain[papan.giliran].isKomputer)
            {
                printf("\nMenunggu giliran %s ...", papan.pemain[papan.giliran].nama);
                posPapan = getInputKomputer();
                papan.waktu = SELESAI;
                sleep(1);
                papan.pemain[papan.giliran].input = posPapan;
                pthread_cancel(thTimer);
                papan.waktu = SELESAI;
                break;
            }
            else
            {
                // Apabila giliran pengguna (bukan komputer), maka perlu memasukkan
                // input posisi petak yang akan diisi dengan simbol miliknya
                if (papan.waktu >= 0) printf("\nMasukkan posisi petak dalam waktu %d detik: ", papan.waktu);
                else printf("\nWaktu habis, masukkan posisi petak manapun untuk melanjutkan: ");
                fgets(input, 4, stdin);

                int newLine = strlen(input) - 1;
                if (input[newLine] == '\n') input[newLine] = '\000';
                upperCase(input);
                posPapan = atoi(input);
                if (strcmp(input, "Q") == 0)
                {
                    // Permainan dijeda terlebih dahulu ketika pemain memilih keluar
                    jeda = true;
                    continue;
                }
                else if (
                    (posPapan > 0 && posPapan <= papan.ukuran * papan.ukuran) &&
                    ((getIsi(posPapan) == '\000') || papan.waktu == TERLAMBAT)
                )
                {
                    pthread_cancel(thTimer);
                    papan.pemain[papan.giliran].input = posPapan;
                    if (papan.waktu > -1) papan.waktu = SELESAI;
                    break;
                }
                else inputValid = false;
            }
        }
    } while (true);
    return NULL;
}

/**
 * Bagian jeda permainan
 */
void jedaPermainan()
{
    clearConsole();
    printf("======================================================");
    printf("\nPermainan %d x %d (%s vs %s)", papan.ukuran, papan.ukuran, papan.pemain[0].nama, papan.pemain[1].nama);
    printf("\n======================================================");
    printf("\n");
    printf("\nKeluar dari permainan ini?");
    if (papan.waktu >= 0) printf("\nWaktu tersisa: %d detik", papan.waktu);
    else printf("\nWaktu habis. Posisi akan diisi secara acak");
    printf("\nTekan 'Enter' untuk keluar atau 'Esc' untuk melanjutkan permainan\n");
    while (true)
    {
        fflush(stdin);

        char input = getch();
        // Opsi 'Enter' untuk keluar permainan
        if ((int)input == KEYBOARD_ENTER)
        {
            pthread_cancel(thTimer);
            papan.waktu = KELUAR;
            jeda = false;
            break;
        }
        // Opsi 'Esc' untuk melanjutkan permainan
        else if ((int)input == KEYBOARD_ESC)
        {
            // Permainan kembali dilanjutkan
            jeda = false;
            break;
        }
    }
}

/**
 * Minimal jumlah simbol sama yang berdampingan yang diperlukan untuk
 * dikategorikan pada status menang
 * @param ukuran ukuran dari papan permainan
 * @return total simbol yang diperlukan
 */
int cekMinimal(int ukuran)
{
    switch (ukuran)
    {
    case 3: return 3; // 3 simbol pada papan 3x3
    case 5: return 4; // 4 simbol pada papan 5x5
    case 7: return 5; // 5 simbol pada papan 7x7
    }
    return 0;
}

/**
 * Cek ketersediaan petak yang kosong pada papan permainan
 * @param simbol simbol pemain
 * @return TRUE jika papan masih bisa diisi, FALSE jika papan tidak lagi dapat diisi
 */
bool cekPapan(char simbol)
{
    if (cekDiagonal(simbol) || cekHorizontal(simbol) || cekVertikal(simbol)) return false;
    // Jika tidak ada petak kosong yang tersisa, maka permainan seri
    else if (getTotalKosong() == 0) 
    {
        papan.giliran = 2;
        return false;
    }
    else return true;
}

/**
 * Cek papan secara diagonal untuk mendeterminasi status permainan
 * @param simbol simbol yang dicek
 * @return TRUE jika secara diagonal telah sesuai, FALSE jika tidak sesuai
 */
bool cekDiagonal(char simbol)
{
    int cek = cekMinimal(papan.ukuran);
    int counter, i, j, temp;

    // Diagonal [/] dari (0, 0) sampai (papan.ukuran-1, 0)
    for (i = 0; i < papan.ukuran; i++)
    {
        counter = 0;
        temp = i;
        j = 0;
        while(temp >= 0)
        {
            if (papan.isi[temp][j] == simbol)
            {
                counter++;
                if (counter >= cek) return true;
            }
            else counter = 0;
            temp--;
            j++;
        }
    }

    // Diagonal [/] dari (0, papan.ukuran-1) sampai (papan.ukuran-1, papan.ukuran-1)
    for (i = 0; i < papan.ukuran; i++)
    {
        counter = 0;
        temp = i;
        j = papan.ukuran - 1;
        while (temp <= papan.ukuran - 1)
        {
            if (papan.isi[temp][j] == simbol)
            {
                counter++;
                if (counter >= cek) return true;
            }
            else counter = 0;
            temp++;
            j--;
        }
    }

    // Diagonal [\] dari (0, papapn.ukuran-1) sampai (papan.ukuran-1, papan.ukuran-1)
    for (i = 0; i < papan.ukuran; i++)
    {
        counter = 0;
        temp = i;
        j = papan.ukuran - 1;
        while (temp >= 0)
        {
            if (papan.isi[temp][j] == simbol)
            {
                counter++;
                if (counter >= cek) return true;
            }
            else counter = 0;
            temp--;
            j--;
        }
    }

    // Diagonal [\] dari (0,0) sampai (papan.ukuran-1, 0)
    for (i = 0; i < papan.ukuran; i++)
    {
        counter = 0;
        temp = i;
        j = 0;
        while (temp < papan.ukuran)
        {
            if (papan.isi[temp][j] == simbol)
            {
                counter++;
                if (counter >= cek) return true;
            }
            else counter = 0;
            temp++;
            j++;
        }
    }

    return false;
}

/**
 * Cek papan secara horizontal untuk mendeterminasi status permainan
 * @param simbol simbol yang dicek
 * @return TRUE jika secara horizontal telah sesuai, FALSE jika tidak sesuai
 */
bool cekHorizontal(char simbol)
{
	int cek = cekMinimal(papan.ukuran);
	int i, j, counter = 0;
	
	for (i = 0; i<papan.ukuran; i++)
    {
		counter = 0;
        for (j = 0; j<papan.ukuran; j++)
        {
            if (papan.isi[i][j] == simbol)
            {
                counter++;
                if (counter >= cek) return true;
            }
            else counter = 0;		
		}
    }
	return false;
}

/**
 * Cek papan secara diagonal untuk mendeterminasi status permainan
 * @param simbol simbol yang dicek
 * @return TRUE jika secara diagonal telah sesuai, FALSE jika tidak sesuai
 */
bool cekVertikal(char simbol)
{
	int cek = cekMinimal(papan.ukuran);
	int i, j, counter = 0;
	
	for (i = 0; i < papan.ukuran; i++)
    {
		counter = 0;
        for (j = 0; j < papan.ukuran; j++)
        {
            if (papan.isi[j][i] == simbol)
            {
                counter++;
                if(counter >= cek) return true;
            }
            else counter = 0;
        }
    }
	return false;
}

/**
 * Menampilkan papan permainan
 */
void showPapan(bool tampilPosisi)
{
    int isi = 1;
    int maxIsi = papan.ukuran * papan.ukuran;
    int maxVertikal = papan.ukuran * 3;
    int isiLen = panjangAngka(maxIsi);
    int i, j;
    int posIsi = 2;
    // Loop setiap barisnya
    for (i = 0; i < maxVertikal + 1; i++)
    {
        bool bisaDiisi = false;
        char cTulisAwal = '|';
        // baris yang bisa diisi adalah kelipatan 3 dimulai dari 2 (posIsi)
        if (i == posIsi)
        {
            bisaDiisi = true;
            posIsi += 3;
        }
        if (i % 3 == 0)
        {
            if (i == 0) cTulisAwal = ' ';
            else cTulisAwal = '|';
        }
        // Loop untuk konten setiap konten pada petak, seperti "____ ", " 1 |", atau "____|"
        for (j = 0; j < papan.ukuran; j++)
        {
            int maxLenIsi = isiLen + 3;
            if (j == 0) maxLenIsi++;
            char cTulis = ' ';
            char cIsi = '_';
            if (j == papan.ukuran - 1) cTulis = cTulisAwal;
            if (i % 3 != 0)
            {
                cTulis = '|';
                cIsi = ' ';
            }
            else if (i % 3 == 0 && i != 0) cTulis = '|';
            char* cGaris = malloc(maxLenIsi + 1);
            memset(cGaris, '\000', maxLenIsi + 1);
            memset(cGaris, cIsi, maxLenIsi - 1);
            memcpy(cGaris + maxLenIsi - 1, &cTulis, 1);
            if (j == 0) memcpy(cGaris, &cTulisAwal, 1);
            if (bisaDiisi)
            {
                int allocSize = isiLen + 1;
                char* isiChar = malloc(allocSize);
                char isiPapan = getIsi(isi);
                int pos = 1;
                if (j == 0) pos++;
                memset(isiChar, '\000', allocSize);
                if (isiPapan != '\000') memcpy(isiChar, &isiPapan, 1);
                else if (tampilPosisi) itoa(isi, isiChar, 10);
                memcpy(cGaris + pos, isiChar, strlen(isiChar));
                free(isiChar);
                isi++;
            }
            int k = 0;
            for (k = 0; k < strlen(cGaris); k++)
            {
                // Pemberian warna merah apabila simbolnya sama dengan simbol pemain ke-1
                if (cGaris[k] == SIMBOL_P1)
                {
                    SetConsoleTextAttribute(hConsole, 12);
                    printf("%c", cGaris[k]);
                    SetConsoleTextAttribute(hConsole, consoleAttr);
                }
                // Pemberian warna hijau apabila simbolnya sama dengan simbol pemain ke-2
                else if (cGaris[k] == SIMBOL_P2)
                {
                    SetConsoleTextAttribute(hConsole, 10);
                    printf("%c", cGaris[k]);
                    SetConsoleTextAttribute(hConsole, consoleAttr);
                }
                else printf("%c", cGaris[k]);
            }
            free(cGaris);
        }
        // Baris baru
        printf("\n");
    }
}

/**
 * Mendapatkan panjang dari suatu angka
 * Misalnya 2 untuk angka 20, atau 3 untuk angka 100
 * @param angka angka yang digunakan untuk mencari panjangnya
 * @return panjang angka
 */
int panjangAngka(int angka)
{
    int len = 0;
    int i = 0;
    while (angka != 0)
    {
        angka /= 10;
        len++;
    }
    return len;
}

/**
 * Mendapatkan isi petak permainan berdasarkan posisi petak tersebut
 * (bukan dari array 2 dimensi)
 * @param pos posisi yang dituju
 * @return karakter yang mengisinya
 */
char getIsi(int pos)
{
    pos--;
    return papan.isi[pos/papan.ukuran][pos%papan.ukuran];
}

/**
 * Mengisi petak permainan berdasarkan posisi petak tersebut
 * @param pos posisi yang dituju
 * @param nilai nilai yang akan diisinya
 */
void setIsi(int pos, char nilai)
{
    pos--;
    papan.isi[pos/papan.ukuran][pos%papan.ukuran] = nilai;
}

/**
 * Mengonversi array-of-char (string) ke bentuk kapital
 * @param data array-of-char yang dituju
 */
void upperCase(char *data)
{
    int i = 0;
    for (i = 0; i < strlen(data); i++)
    {
        *(data + i) = toupper(*(data + i));
    }
}

/**
 * Batasan jangkauan yang bisa diprediksi oleh komputer
 * @param kesulitan tingkat kesulitan pada permainan
 * @param ukuran ukuran papan permainan
 * @return batasan jangkauan
 */
int getJangkauan(int kesulitan, int ukuran)
{
    switch (kesulitan)
    {
    case 2:
    // Tingkat sedang
        if (ukuran == 3) return 2; // ukuran papan 3x3
        if (ukuran == 5) return 2; // ukuran papan 5x5
        if (ukuran == 7) return 2; // ukuran papan 7x7
    case 3:
    // Tingkat sulit
        if (ukuran == 3) return 4; // ukuran papan 3x3
        if (ukuran == 5) return 4; // ukuran papan 5x5
        if (ukuran == 7) return 3; // ukuran papan 7x7
    default: return 0;
    }
}

/**
 * Mendapatkan input yang dilakukan oleh komputer dalam mengisi
 * petak pada papan permainan.
 * @return posisi petak yang dipilih komputer
 */
int getInputKomputer()
{
    int input = 0;
    int jangkauan = getJangkauan(papan.kesulitan, papan.ukuran);
    switch (papan.kesulitan)
    {
    case 1: // Tingkat mudah
        input = isiAcak();
        break;
    case 2: // Tingkat sedang
        input = isiKritis(jangkauan);
        break;
    case 3: // Tingkat sulit
        input = isiKritis(jangkauan);
        break;
    }
    return input;
}

/**
 * Mendapatkan posisi acak untuk mengisi papan permainan
 * @return posisi acak
 */
int isiAcak()
{
    srand(time(NULL));
    int range = papan.ukuran * papan.ukuran;
    int min = 1;
    int pos = 0;
    do { pos = (rand() % range) + min; }
    while (getIsi(pos) != '\000');
    return pos;
}

/**
 * Algoritma minimax untuk memprediksi gerakan selanjutnya yang digunakan
 * untuk mendapatkan posisi terbaik
 * @param isMax jika pemain yang memaksimalkan kemenangan adalah komputer
 * @param jangakauan total jangkauan yang sudah dilakukan
 * @param alpha alpha
 * @param beta beta
 * @param maxJangkauan batasan jangkauan yang perlu dilakukan
 * @return skor terbaik pada posisi terkait
 */
int minimax(bool isMax, int jangkauan, int alpha, int beta, int maxJangkauan)
{
    int giliranLawan = (papan.giliran == 1)? 0 : 1;
    int skor = eval();
    if (skor == 10) return skor - jangkauan;
    else if (skor == -10) return skor + jangkauan;
    else if (!isBisaDiisi()) return 0;

    if (jangkauan >= maxJangkauan) return 0;
    int skorX = (isMax)? -10000 : 10000;
    char simbol = (isMax)? papan.pemain[papan.giliran].simbol : papan.pemain[giliranLawan].simbol;
    int i, j;
    if (isMax)
    {
        for (i = 0; i < papan.ukuran; i++)
        {
            for (j = 0; j < papan.ukuran; j++)
            {
                if (papan.isi[i][j] == '\000')
                {
                    papan.isi[i][j] = simbol;
                    int skor = minimax(!isMax, jangkauan + 1, alpha, beta, maxJangkauan);
                    papan.isi[i][j] = '\000';
                    skorX = max(skor, skorX);
                    alpha = max(skor, alpha);
                    if (alpha >= beta) return alpha;
                }
            }
        }
        return alpha;
    }
    else
    {
        for (i = 0; i < papan.ukuran; i++)
        {
            for (j = 0; j < papan.ukuran; j++)
            {
                if (papan.isi[i][j] == '\000')
                {
                    papan.isi[i][j] = papan.pemain[giliranLawan].simbol;
                    int skor = minimax(!isMax, jangkauan + 1, alpha, beta, maxJangkauan);
                    papan.isi[i][j] = '\000';
                    skorX = min(skor, skorX);
                    beta = min(skor, beta);
                    if (beta <= alpha) return beta;
                }
            }
        }
        return beta;
    }
}

/**
 * Mendapatkan posisi kritis untuk mengisi papan permainan
 * @param maxJangkauan batasan jangkauan yang perlu dilakukan
 * @return posisi petak yang bernilai kritis
 */
int isiKritis(int maxJangkauan)
{
    if (
        ((papan.ukuran == 5) && (getTotalKosong() > START_MINIMAX_5x5)) ||
        ((papan.ukuran == 7) && (getTotalKosong() > START_MINIMAX_7x7))
    ) return isiAcak();
    int skorX = -10000;
    int posX = -1;
    int posY = -1;
    int jangkauan = 0;
    int alpha = -10000;
    int beta = 10000;

    int i, j;
    for (i = 0; i < papan.ukuran; i++)
    {
        for (j = 0; j < papan.ukuran; j++)
        {
            if (papan.isi[i][j] == '\000')
            {
                papan.isi[i][j] = papan.pemain[papan.giliran].simbol;
                int skor = minimax(false, jangkauan, alpha, beta, maxJangkauan);
                papan.isi[i][j] = '\000';
                if (skor > skorX)
                {
                    posX = i;
                    posY = j;
                    skorX = skor;
                }
            }
        }
    }
    return parsePosisi(posX, posY);
}

/**
 * Determinasi apabila petak masih bisa diisi atau tidak
 * @return TRUE jika masih bisa diisi, FALSE jika tidak bisa
 */
bool isBisaDiisi()
{
    int i, j;
    for (i = 0; i < papan.ukuran; i++)
    {
        for (j = 0; j < papan.ukuran; j++)
        {
            if (papan.isi[i][j] == '\000') return true;
        }
    }
    return false;
}

/**
 * Mengubah koordinat (x, y) pada array menjadi posisi petak pada papan permainan
 * @param x posisi absis [X][...]
 * @param y posisi ordinat [...][Y]
 * @return posisi petak pada permainan
 */
int parsePosisi(int x, int y)
{
    int hasil = 0;
    int i, j;
    for (i = 0; i < papan.ukuran; i++)
    {
        for (j = 0; j < papan.ukuran; j++)
        {
            if (i == x && j == y) return hasil + 1;
            hasil++;
        }
    }
    return 0;
}

/**
 * Mendapatkan jumlah petak yang kosong pada papan permainan
 * @return jumlah petak yang kosong
 */
int getTotalKosong()
{
    int counter = 0;
    int i;
    for (i = 1; i < papan.ukuran * papan.ukuran + 1; i++)
    {
        if (getIsi(i) == '\000') counter++;
    }
    return counter;
}

/**
 * Sajian ketika pemain berhasil memenangkan permainan melawan komputer
 * @param status index pemenang dari permainan (0 = pemain ke-1, 1 = pemain ke-2)
 */
void menuMenang(int status)
{
    char input = '\000';
    printf("\n%s telah memenangkan permainan", papan.pemain[0].nama);
    printf("\nSkor Baru: %d (+100)", papan.pemain[0].skor);
    printf("\nQ: Ke menu utama | R: Lanjutkan Permainan | S: Simpan Permainan");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    switch (input)
    {
    case 'Q':
        return menuUtama();
        break;
    case 'R':
        return setPermainan();
        break;
    case 'S':
        return menuSimpan();
        break;
    default:
        inputValid = false;
        menuGameOver(status);
        break;
    }
    menuMenang(status);
}


/**
 * Menampilkan menu untuk menyimpan skor pemain ketika kalah dan skor
 * pemain melampaui skor yang ada di daftar 10 pemain dengan skor tertinggi
 * yang ditampilkan apabila pemain ingin menyimpan skornya
 */
void menuSaveSkor()
{
    int skor = papan.pemain[0].skor;
    clearConsole();
    printf("======================================================");
    printf("\nDaftar 10 Pemain dengan Skor Tertinggi");
    printf("\n======================================================");
    printf("\n");
    printf("\nSkor anda: %d", papan.pemain[0].skor);
    showSkor();
    printf("\nHarap masukkan nama anda untuk melanjutkan");
    printf("\n");
    pesanInvalid("\nNama harus memiliki (3-20) karakter");
    printf("\nMasukkan nama: ");
    fflush(stdin);
    fgets(papan.pemain[0].nama, 25, stdin);
    if (papan.pemain[0].nama[strlen(papan.pemain[0].nama) - 1] == '\n') papan.pemain[0].nama[strlen(papan.pemain[0].nama) - 1] = '\000';
    if (strlen(papan.pemain[0].nama) < 3 || strlen(papan.pemain[0].nama) >= 25)
    {
        inputValid = false;
        return menuSaveSkor();
    }
    clearConsole();
    printf("======================================================");
    printf("\nDaftar 10 Pemain dengan Skor Tertinggi");
    printf("\n======================================================");
    printf("\n");
    int status = saveSkor();
    if (status == 0) showSkor();
    papan.pemain[0].skor = 0;
    if (idPapan != 0) savePermainan(idPapan);
    printf("\n");
    printf("\nTekan tombol apapun untuk melanjutkan");
    while (true)
    {
        char c = getch();
        if ((int)c) return menuUtama();
    }
}

/**
 * Sajian ketika pemain kalah dalam permainan melawan komputer
 * @param status index pemenang dari permainan (0 = pemain ke-1, 1 = pemain ke-2)
 */
void menuKalah(int status)
{
    char input = '\000';
    int skor = papan.pemain[0].skor;
    printf("\n%s kalah dalam permainan", papan.pemain[0].nama);
    printf("\nSkor: %d", papan.pemain[0].skor);
    printf("\nQ: Ke menu utama | R: Ulangi Permainan");
    if (skor > sInfo[9].skor) printf(" | S: Simpan Skor");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    if (input == 'Q')
    {
        papan.pemain[0].skor = 0;
        if (idPapan != 0) savePermainan(idPapan);
        return menuUtama();
    }
    else if (input == 'R')
    {
        papan.pemain[0].skor = 0;
        if (idPapan != 0) savePermainan(idPapan);
        return setPermainan();
    }
    else if (skor > sInfo[9].skor && input == 'S') return menuSaveSkor();
    else
    {
        inputValid = false;
        menuGameOver(status);
    }
}

/**
 * Sajian ketika pemain mendapatkan hasil yang seri ketika melawan komputer
 * @param status index pemenang dari permainan (0 = pemain ke-1, 1 = pemain ke-2)
 */
void menuSeri(int status)
{
    char input = '\000';
    printf("\nPermainan seri", papan.pemain[0].nama);
    printf("\nSkor: %d", papan.pemain[0].skor);
    printf("\nQ: Ke menu utama | R: Ulangi Permainan");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    switch (input)
    {
    case 'Q':
        return menuUtama();
        break;
    case 'R':
        return setPermainan();
        break;
    default:
        inputValid = false;
        menuGameOver(status);
        break;
    }
}

/**
 * Menu yang ditampilkan apabila pemain ingin menyimpan permainannya
 * untuk mode permainan melawan komputer, digunakan untuk meminta pilihan
 * lokasi (ID) yang digunakan untuk menyimpannya
 */
void menuSimpan()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nSimpan Permainan");
    printf("\n======================================================");
    printf("\n");
    printf("\nPilihlah lokasi di mana anda akan menyimpan permainan");
    showPermainanLama();
    printf("\nQ: Batalkan");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    if (input == 'Q') menuGameOver(0);
    
    int opsi = atoi(&input);
    if (opsi > 0 && opsi <= MAX_SAVEGAME) menuSimpanSiap(opsi);
    else
    {
        inputValid = false;
        return menuSimpan();
    }
}

/**
 * Proses selanjutnya setelah memilih pilihan lokasi yang digunakan untuk
 * menyimpan menu permainan
 * @param posisi posisi (ID) di mana permainan disimpan
 */
void menuSimpanSiap(int posisi)
{
    Papan temp;
    char input[25] = {'\000'};
    char kesulitan[10] = {'\000'};
    clearConsole();
    memcpy(kesulitan, getKesulitanStr(papan.kesulitan), strlen(getKesulitanStr(papan.kesulitan)));
    printf("======================================================");
    printf("\nSimpan Permainan");
    printf("\n======================================================");
    printf("\n");
    printf("\nBerikut merupakan data permainan yang akan anda simpan");
    printf("\nTingkat Kesulitan : %s", kesulitan);
    printf("\nUkuran Papan      : %d x %d", papan.ukuran, papan.ukuran);
    if (imporPermainan(posisi, &temp) == 0) printf("\n[Peringatan! Anda akan menimpa data permainan ini]");
    printf("\n");
    pesanInvalid("\nLabel harus memiliki (3-20) karakter");
    printf("\nMasukkan label penyimpanan: ");
    fflush(stdin);
    fgets(input, 25, stdin);
    if (input[strlen(input) - 1] == '\n') input[strlen(input) - 1] = '\000';
    if (strlen(input) < 3 || strlen(input) >= 25)
    {
        inputValid = false;
        menuSimpanSiap(posisi);
    }
    memset(papan.nama, '\000', sizeof(papan.nama));
    memcpy(papan.nama, input, strlen(input));

    int status = savePermainan(posisi);
    if (status == 0) return menuSimpanSelesai();
}

/**
 * Menu yang ditampilkan ketika permainan telah disimpan
 */
void menuSimpanSelesai()
{
    char input = '\000';
    char kesulitan[10] = {'\000'};
    clearConsole();
    memcpy(kesulitan, getKesulitanStr(papan.kesulitan), strlen(getKesulitanStr(papan.kesulitan)));
    printf("======================================================");
    printf("\nSimpan Permainan");
    printf("\n======================================================");
    printf("\n");
    printf("\nData permainan ini telah disimpan");
    printf("\nLabel Papan       : %s", papan.nama);
    printf("\nTingkat Kesulitan : %s", kesulitan);
    printf("\nUkuran Papan      : %d x %d", papan.ukuran, papan.ukuran);
    printf("\nQ: Ke menu utama | R: Lanjutkan Permainan");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    switch (input)
    {
    case 'Q':
        return menuUtama();
        break;
    case 'R':
        return setPermainan();
    default:
        inputValid = false;
        menuSimpanSelesai();
        break;
    }
}

/**
 * Menu yang ditampilkan ketika permainan berakhir pada mode permainan
 * multipemain
 * @param status index pemenang dari permainan (0 = pemain ke-1, 1 = pemain ke-2)
 */
void menuMultipemain(int status)
{
    char input = '\000';
    if (status == 1 || status == 0)
    {
        int pemenang = status;
        int kalah = (pemenang == 0)? 1 : 0;
        printf("\n%s telah mengalahkan %s", papan.pemain[pemenang].nama, papan.pemain[kalah].nama);
    }
    else printf("\nPermainan seri", papan.pemain[0].nama);
    printf("\nQ: Ke menu utama | R: Ulangi Permainan");
    printf("\n");
    pesanInvalid("Pilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    switch (input)
    {
    case 'Q':
        return menuUtama();
        break;
    case 'R':
        return setPermainan();
        break;
    case 'S':
        return menuSimpan();
        break;
    default:
        inputValid = false;
        menuGameOver(status);
        break;
    }
}

/**
 * Menu yang ditampilkan ketika permainan berakhir
 * @param status index pemenang dari permainan (0 = pemain ke-1, 1 = pemain ke-2)
 */
void menuGameOver(int status)
{
    clearConsole();
    printf("======================================================");
    printf("\nPermainan %d x %d (%s vs %s)", papan.ukuran, papan.ukuran, papan.pemain[0].nama, papan.pemain[1].nama);
    printf("\n======================================================");
    printf("\n");
    showPapan(false);
    if (papan.pemain[1].isKomputer)
    {
        switch (status)
        {
        case 0: // menang lawan komputer
            menuMenang(status);
            break;
        case 1: // kalah lawan komputer
            menuKalah(status);
        default: // seri
            menuSeri(status);
            break;
        }
    }
    else menuMultipemain(status);
}

/**
 * Bagian yang ditampilkan ketika pengguna memilih untuk melihat
 * cara bermain pada permainan ini
 */
void bagianCara()
{
    char input = '\000';
    char fileCara[255] = {'\000'};
    FILE *in;
    clearConsole();
	if((in=fopen(PATH_CARA, "r")) == NULL)
    {
        printf("Error Opening File"); //tutup program karena file ga ada
		fclose(in);
	}
    //baca isi file dengan gets lalu simpan ke tampil
	while(fgets(fileCara, sizeof(fileCara), in)) printf("%s", &fileCara);
    fclose(in);
    printf("\n");
    printf("\nTekan tombol apapun untuk ke menu utama");
    while (true)
    {
        fflush(stdin);

        char input = getch();
        if((int)input) return menuUtama();
    }
}

/**
 * Menampilkan data skor yang tersimpan pada file
 */
void showSkor()
{
    memset(sInfo, 0, sizeof(sInfo));
    int status = imporSkor();
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        printf("\n[%d.] ", i + 1);
        if (strlen(sInfo[i].nama) == 0) printf("%-8.25s\t | %-5.10s | %s", "Kosong", "-", "-");
        else printf("%-8.25s\t | %-5.10s | %d", sInfo[i].nama, sInfo[i].kesulitan, sInfo[i].skor);
    }
}

/**
 * Menu yang digunakan untuk menampilkan bagian daftar 10 pemain dengan
 * skor tertinggi
 */
void bagianSkor()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nDaftar 10 Pemain dengan Skor Tertinggi");
    printf("\n======================================================");
    printf("\n");
    showSkor();
    printf("\nQ: Kembali | D: Hapus Skor");
    printf("\n");
    pesanInvalid("\nPilihan anda tidak valid");
    printf("\nMasukkan pilihan: ");
    getInput(&input);
    switch (input)
	{
    case 'Q': return menuUtama();
    case 'D': return menuHapusSkor();
    default:
        inputValid = false;
        return bagianSkor();
    break;
	}
}

/**
 * Mengimpor data skor yang digunakan untuk menampilkan daftar 10 pemain
 * dengan skor tertinggi yang tersimpan pada file
 * @return 0 jika sukses, 1 jika gagal
 */
int imporSkor()
{
    int skorSize = sizeof(sInfo);
    char fileData[255] = {'\000'};
    char *fileLokasi = PATH_SKOR;
    char *dataSkor = malloc(skorSize);
    memset(dataSkor, 0, skorSize);
    memcpy(fileData, fileLokasi, strlen(fileLokasi));
    memcpy(fileData + strlen(fileData), ".dat", 4);
    
    FILE *pFile = fopen(fileData, "rb");
    if (pFile == NULL) return 1;
    if (fread(dataSkor, skorSize, 1, pFile) != 1) return 1;
    memset(&sInfo, 0, sizeof(sInfo));
    reverse(dataSkor, skorSize);
    memcpy(sInfo, dataSkor, skorSize);
    free(dataSkor);
    fclose(pFile);
    return 0;
}

/**
 * Menambahkan skor terbaru pada daftar 10 pemain dengan skor tertinggi
 * dan menyimpannya pada file
 * @return 0 jika sukses, 1 jika gagal
 */
int saveSkor()
{
    memset(sInfo[9].nama, 0, sizeof(sInfo[9].nama));
    memset(sInfo[9].kesulitan, 0, sizeof(sInfo[9].kesulitan));
    memcpy(sInfo[9].nama, papan.pemain[0].nama, strlen(papan.pemain[0].nama));
    memcpy(sInfo[9].kesulitan, getKesulitanStr(papan.kesulitan), strlen(getKesulitanStr(papan.kesulitan)));
    sInfo[9].skor = papan.pemain[0].skor;
    sortSkor();

    int skorSize = sizeof(sInfo);
    char fileData[255] = {'\000'};
    char *fileLokasi = PATH_SKOR;
    char *dataSkor = malloc(skorSize);
    memset(dataSkor, 0, skorSize);
    memcpy(fileData, fileLokasi, strlen(fileLokasi));
    memcpy(fileData + strlen(fileData), ".dat", 4);
    memcpy(dataSkor, sInfo, skorSize);
    reverse(dataSkor, skorSize);

    FILE *pFile = fopen(fileData, "wb");
    if (pFile == NULL) return 1;
    if (fwrite(dataSkor, skorSize, 1, pFile) != 1) return 1;
    fclose(pFile);
    free(dataSkor);
    return 0;
}

/**
 * Menyortir daftar 10 pemain dengan skor tertinggi terurut berdasarkan
 * skor tertinggi (terurut dari yang lebih dahulu tersimpannya)
 */
void sortSkor()
{
    SkorInfo temp;
    int i, j;

    int totalSkor = 10;
    for (i = 0; i < totalSkor; i++)
    {
        for (j = totalSkor - 1; j > i; j--)
        {
            if (sInfo[j].skor > sInfo[j - 1].skor)
            {
                memset(&temp, 0, sizeof(temp));
				memcpy(temp.nama, sInfo[j].nama, strlen(sInfo[j].nama));
                memcpy(temp.kesulitan, sInfo[j].kesulitan, strlen(sInfo[j].kesulitan));
                temp.skor = sInfo[j].skor;

                memset(sInfo[j].nama, 0, sizeof(sInfo[j].nama));
                memset(sInfo[j].kesulitan, 0, sizeof(sInfo[j].kesulitan));
                memcpy(sInfo[j].nama, sInfo[j - 1].nama, strlen(sInfo[j - 1].nama));
                memcpy(sInfo[j].kesulitan, sInfo[j - 1].kesulitan, strlen(sInfo[j - 1].kesulitan));
                sInfo[j].skor = sInfo[j - 1].skor;

                memset(sInfo[j - 1].nama, 0, sizeof(sInfo[j - 1].nama));
                memset(sInfo[j - 1].kesulitan, 0, sizeof(sInfo[j - 1].kesulitan));
                memcpy(sInfo[j - 1].nama, temp.nama, strlen(temp.nama));
                memcpy(sInfo[j - 1].kesulitan, temp.kesulitan, strlen(temp.kesulitan));
                sInfo[j - 1].skor = temp.skor;
			}
        }
    }
}

/**
 * Menghapus file yang digunakan untuk menyimpan daftar 10 pemain
 * dengan skor tertinggi
 * @return 0 jika sukses, selain itu gagal
 */
int deleteSkor()
{
    char fileData[255] = {'\000'};
    char *fileLokasi = PATH_SKOR;
    memcpy(fileData, fileLokasi, strlen(fileLokasi));
    memcpy(fileData + strlen(fileData), ".dat", 4);
    return remove(fileData);
}

/**
 * Menu yang digunakan sebagai penyampaian pesan konfirmasi sebelum
 * semua daftar 10 pemain dengan skor tertinggi terhapus (sebelum file
 * yang digunakan untuk menyimpan data tersebut dihapus)
 */
void menuHapusSkor()
{
    clearConsole();
    printf("======================================================");
    printf("\nDaftar 10 Pemain dengan Skor Tertinggi");
    printf("\n======================================================");
    printf("\n");
    printf("\nApakah anda ingin menghapus semua data skor?");
    printf("\nTekan 'Enter' untuk setuju atau 'Esc' untuk ke menu utama");
    while (true)
    {
        fflush(stdin);
        char input = getch();
        if ((int)input == KEYBOARD_ENTER)
        {
            int status = deleteSkor();
            return menuUtama();
        }
        else if ((int)input == KEYBOARD_ESC) return bagianSkor();
    }
}

/**
 * Menampilkan pesan konfirmasi ketika pengguna ingin keluar dari
 * permainan (program)
 */
void bagianKeluar()
{
    char input = '\000';
    clearConsole();
    printf("======================================================");
    printf("\nKeluar Permainan");
    printf("\n======================================================");
    printf("\n");
    printf("\nApakah anda yakin ingin keluar dari permainan?");
    printf("\nTekan 'Enter' untuk melanjutkan atau 'Esc' untuk kembali");
    while (true)
    {
        fflush(stdin);
        char input = getch();
        if ((int)input == KEYBOARD_ENTER) exit(0);
        if ((int)input == KEYBOARD_ESC) return menuUtama();
    }
}

/**
 * Bagian dari fungsi minimax, digunakan untuk mengevaluasi papan permainan
 * sehingga dapat memberikan hasil skor yang didapatkan ketika diisi oleh
 * simbol pemain di mana menentukan kondisi pada permainan.
 * @return 10 jika hasilnya pemain ke-1 yang menang,
 * -10 jika hasilnya adalah pemain ke-2 yang menang,
 * 0 ketika hasilnya seri
 */
int eval()
{
    int giliran = papan.giliran;
    char cek = SIMBOL_P1;
    char simbolPemain = papan.pemain[papan.giliran].simbol;
    bool status = cekPapan(cek);
    if (status)
    {
        cek = SIMBOL_P2;
        status = cekPapan(cek);
    }
    if (!status)
    {
        if (papan.giliran == 2)
        {
            papan.giliran = giliran;
            return 0;
        }
        if (cek == simbolPemain) return 10;
        else return -10;
    }
    return 0;
}
