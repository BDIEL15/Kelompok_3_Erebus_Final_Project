#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define KAPASITAS_AWAL 4  /* initial heap capacity; doubles on every resize */

typedef struct {
    char nama[100];
    int jumlahKorban;
    int jarak;
    int kelompokRentan;
    int skorPrioritas;
    int sudahDibantu;
} Daerah;

/* ─────────────────────────────────────────────
   I/O helpers
   ───────────────────────────────────────────── */

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * Reads a validated integer.
 * minVal : inclusive lower bound
 * maxVal : inclusive upper bound (pass -1 to skip)
 */
int bacaInt(const char *prompt, int minVal, int maxVal) {
    int nilai;
    char sisa;
    int valid;

    do {
        printf("%s", prompt);
        valid = scanf("%d%c", &nilai, &sisa);

        if (valid != 2 || sisa != '\n') {
            printf("  [!] Input tidak valid. Masukkan angka bulat.\n");
            clearInputBuffer();
            continue;
        }
        if (nilai < minVal) {
            printf("  [!] Nilai minimal adalah %d.\n", minVal);
            continue;
        }
        if (maxVal != -1 && nilai > maxVal) {
            printf("  [!] Nilai maksimal adalah %d.\n", maxVal);
            continue;
        }
        break;

    } while (1);

    return nilai;
}

/* Reads a non-empty, non-whitespace-only string. */
void bacaString(const char *prompt, char *dest, int maxLen) {
    int valid, i;

    do {
        printf("%s", prompt);

        if (fgets(dest, maxLen, stdin) == NULL) {
            dest[0] = '\0';
            continue;
        }

        dest[strcspn(dest, "\n")] = '\0';

        valid = 0;
        for (i = 0; dest[i] != '\0'; i++) {
            if (!isspace((unsigned char)dest[i])) {
                valid = 1;
                break;
            }
        }

        if (!valid)
            printf("  [!] Nama tidak boleh kosong.\n");

    } while (!valid);
}

/* ─────────────────────────────────────────────
   Dynamic array: grow capacity when needed
   ───────────────────────────────────────────── */

/*
 * Tries to double the capacity of *data.
 * Returns 1 on success, 0 on allocation failure.
 */
int expandArray(Daerah **data, int *kapasitas) {
    int kapasitasBaru = (*kapasitas) * 2;
    Daerah *tmp = (Daerah *)realloc(*data, kapasitasBaru * sizeof(Daerah));

    if (tmp == NULL) {
        printf("\n[!] Gagal mengalokasikan memori tambahan.\n");
        return 0;
    }

    *data      = tmp;
    *kapasitas = kapasitasBaru;
    printf("  [i] Kapasitas diperluas menjadi %d slot.\n", kapasitasBaru);
    return 1;
}

/* ─────────────────────────────────────────────
   Business logic
   ───────────────────────────────────────────── */

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

    if (d->skorPrioritas < 0)
        d->skorPrioritas = 0;
}

void sortData(Daerah *data, int n) {
    int i, j;
    Daerah temp;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (data[j].skorPrioritas < data[j + 1].skorPrioritas) {
                temp       = data[j];
                data[j]    = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

/*
 * Adds a new entry. Grows the array automatically if full.
 * Returns 1 on success, 0 if memory expansion failed.
 */
int tambahDaerah(Daerah **data, int *n, int *kapasitas) {

    /* Expand before we run out of space */
    if (*n >= *kapasitas) {
        if (!expandArray(data, kapasitas))
            return 0;
    }

    printf("\n=== Tambah Data Daerah ===\n");
    clearInputBuffer();   /* consume newline left by menu scanf */

    bacaString("Nama daerah: ", (*data)[*n].nama, sizeof((*data)[*n].nama));

    (*data)[*n].jumlahKorban   = bacaInt("Jumlah korban: ", 0, -1);
    (*data)[*n].jarak          = bacaInt("Jarak dari pusat bantuan (km): ", 0, -1);
    (*data)[*n].kelompokRentan = bacaInt("Jumlah lansia/anak-anak: ", 0, -1);

    /* kelompokRentan cannot exceed jumlahKorban */
    while ((*data)[*n].kelompokRentan > (*data)[*n].jumlahKorban) {
        printf("  [!] Jumlah lansia/anak-anak tidak boleh melebihi jumlah korban (%d).\n",
               (*data)[*n].jumlahKorban);
        (*data)[*n].kelompokRentan = bacaInt("Jumlah lansia/anak-anak: ", 0, -1);
    }

    (*data)[*n].sudahDibantu = 0;
    hitungPrioritas(&(*data)[*n]);
    (*n)++;

    printf("Data berhasil ditambahkan! (total: %d, kapasitas: %d)\n",
           *n, *kapasitas);
    return 1;
}

void tampilkanData(Daerah *data, int n) {
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

void tandaiDibantu(Daerah *data, int n) {
    int pilih;

    if (n == 0) {
        printf("\nBelum ada data.\n");
        return;
    }

    tampilkanData(data, n);

    pilih = bacaInt("\nPilih nomor daerah yang sudah dibantu: ", 1, n);

    if (data[pilih - 1].sudahDibantu) {
        printf("[!] Daerah ini sudah ditandai sebelumnya.\n");
    } else {
        data[pilih - 1].sudahDibantu = 1;
        printf("Daerah berhasil ditandai sudah dibantu.\n");
    }
}

/* ─────────────────────────────────────────────
   Entry point
   ───────────────────────────────────────────── */

int main() {
    int n         = 0;
    int kapasitas = KAPASITAS_AWAL;
    int pilihan;

    /* Initial heap allocation */
    Daerah *data = (Daerah *)malloc(kapasitas * sizeof(Daerah));
    if (data == NULL) {
        printf("[!] Gagal mengalokasikan memori awal. Program berhenti.\n");
        return 1;
    }

    do {
        printf("\n=================================================\n");
        printf("     SISTEM DISTRIBUSI BANTUAN BENCANA\n");
        printf("=================================================\n");
        printf("1. Tambah daerah terdampak\n");
        printf("2. Tampilkan daftar prioritas\n");
        printf("3. Tandai daerah sudah dibantu\n");
        printf("0. Keluar\n");

        pilihan = bacaInt("Pilih menu: ", 0, 3);

        switch (pilihan) {
            case 1:
                tambahDaerah(&data, &n, &kapasitas);
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
        }

    } while (pilihan != 0);

    /* Release heap memory before exit */
    free(data);
    data = NULL;

    return 0;
}