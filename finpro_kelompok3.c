#include <stdio.h>
#include <string.h>

#define MAX 100

typedef struct {
    char nama[100];
    int jumlahKorban;
    int jarak;
    int kelompokRentan;
    int skorPrioritas;
    int sudahDibantu;
} Daerah;

void hitungPrioritas(Daerah *d) {

    /*
       Rumus prioritas:
       - jumlah korban lebih banyak -> prioritas naik
       - kelompok rentan lebih banyak -> prioritas naik
       - jarak lebih jauh -> prioritas sedikit turun
    */

    d->skorPrioritas =
        (d->jumlahKorban * 2) +
        (d->kelompokRentan * 5) -
        d->jarak;

    if (d->skorPrioritas < 0) {
        d->skorPrioritas = 0;
    }
}

void sortData(Daerah data[], int n) {

    int i, j;
    Daerah temp;

    for (i = 0; i < n - 1; i++) {

        for (j = 0; j < n - i - 1; j++) {

            if (data[j].skorPrioritas < data[j + 1].skorPrioritas) {

                temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;

            }
        }
    }
}

void tambahDaerah(Daerah data[], int *n) {

    printf("\n=== Tambah Data Daerah ===\n");

    getchar();

    printf("Nama daerah: ");
    fgets(data[*n].nama, sizeof(data[*n].nama), stdin);

    data[*n].nama[strcspn(data[*n].nama, "\n")] = '\0';

    printf("Jumlah korban: ");
    scanf("%d", &data[*n].jumlahKorban);

    printf("Jarak dari pusat bantuan (km): ");
    scanf("%d", &data[*n].jarak);

    printf("Jumlah lansia/anak-anak: ");
    scanf("%d", &data[*n].kelompokRentan);

    data[*n].sudahDibantu = 0;

    hitungPrioritas(&data[*n]);

    (*n)++;

    printf("Data berhasil ditambahkan!\n");
}

void tampilkanData(Daerah data[], int n) {

    int i;

    if (n == 0) {
        printf("\nBelum ada data.\n");
        return;
    }

    printf("\n================ DAFTAR PRIORITAS BANTUAN ================\n");

    printf("--------------------------------------------------------------------------\n");
    printf("No Nama Daerah      Korban Jarak Rentan Skor Status\n");
    printf("--------------------------------------------------------------------------\n");

    for (i = 0; i < n; i++) {

        printf("%-2d %-16s %-7d %-6d %-7d %-5d %s\n",
               i + 1,
               data[i].nama,
               data[i].jumlahKorban,
               data[i].jarak,
               data[i].kelompokRentan,
               data[i].skorPrioritas,
               data[i].sudahDibantu ? "Sudah" : "Belum");

    }
}

void tandaiDibantu(Daerah data[], int n) {

    int pilih;

    if (n == 0) {
        printf("\nBelum ada data.\n");
        return;
    }

    tampilkanData(data, n);

    printf("\nPilih nomor daerah yang sudah dibantu: ");
    scanf("%d", &pilih);

    if (pilih >= 1 && pilih <= n) {

        data[pilih - 1].sudahDibantu = 1;

        printf("Daerah berhasil ditandai sudah dibantu.\n");

    } else {

        printf("Nomor tidak valid.\n");

    }
}

int main() {

    Daerah data[MAX];

    int n = 0;
    int pilihan;

    do {

        printf("\n=================================================\n");
        printf("     SISTEM DISTRIBUSI BANTUAN BENCANA\n");
        printf("=================================================\n");
        printf("1. Tambah daerah terdampak\n");
        printf("2. Tampilkan daftar prioritas\n");
        printf("3. Tandai daerah sudah dibantu\n");
        printf("0. Keluar\n");

        printf("Pilih menu: ");
        scanf("%d", &pilihan);

        switch (pilihan) {

            case 1:
                tambahDaerah(data, &n);
                sortData(data, n);
                break;

            case 2:
                sortData(data, n);
                tampilkanData(data, n);
                break;

            case 3:
                tandaiDibantu(data, n);
                break;

            case 0:
                printf("\nProgram selesai.\n");
                break;

            default:
                printf("\nPilihan tidak valid.\n");

        }

    } while (pilihan != 0);

    return 0;
}