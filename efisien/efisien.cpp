// ============================================================================
// penjadwalan_efisien.cpp
// VERSI MODIFIKASI (efisien): penempatan mata kuliah ke slot waktu dengan
// ALIRAN MAKSIMUM (max-flow, Dinic) + penugasan ruang best-fit.
//
// Ide modifikasi, dibanding Welsh-Powell greedy pada file aslinya:
//   Masalah greedy kelas-per-kelas: urutan derajat membuat kelompok kecil
//   "kelaparan" (baru terlayani setelah kelompok besar selesai), sehingga
//   ekor jadwal tipis (4 matkul/slot) dan total membengkak jadi 20 slot.
//   Versi ini mengganti pewarnaan greedy dengan konstruksi global yang
//   terbukti optimal untuk struktur masalah ini:
//
//   1. Kelompok konflik = satu (prodi, kelas). Di dalam satu kelompok,
//      semua matkul saling bentrok (klik), antar kelompok tidak ada sisi.
//   2. Bangun jaringan alir: source -> kelompok (kapasitas = ukuran
//      kelompok), kelompok -> slot (kapasitas 1, artinya satu kelompok
//      menaruh paling banyak satu matkul per slot), slot -> sink
//      (kapasitas = jumlah ruang). Aliran maksimum = penempatan yang
//      menaruh matkul sebanyak-banyaknya tanpa melanggar konflik dan
//      kapasitas ruang per slot.
//   3. Cari S (jumlah slot) TERKECIL yang feasible, mulai dari batas
//      bawah teoritis: maks(ukuran kelompok terbesar,
//      ceil(total matkul / jumlah ruang)). Bila aliran maksimum == total
//      matkul, S tersebut optimal (tidak mungkin lebih kecil).
//   4. Tetapkan ruang per slot secara best-fit: matkul terbesar dulu,
//      ruang bebas terkecil yang masih muat. Kapasitas tidak pernah
//      mengikat di data ini (hanya satu kelompok yang punya matkul > 35,
//      dan satu kelompok menaruh paling banyak satu matkul per slot),
//      sehingga penugasan ruang selalu berhasil.
//
// Keluaran: jadwal final yang valid (tanpa konflik kelompok, tanpa dobel
// ruang, kapasitas muat) dengan slot seminimal mungkin.
//
// Benchmark: sama seperti file original, program mengulang solve()
// sebanyak ULANG kali dan melaporkan rata-rata waktu per run, plus
// statistik aliran Dinic (jumlah BFS dan kunjungan edge DFS).
//
// Compile:  g++ -std=c++17 -O2 -o jadwal_efisien penjadwalan_efisien.cpp
// Jalankan: ./jadwal_efisien
// ============================================================================

#include <bits/stdc++.h>
using namespace std;

struct MataKuliah {
    string kode, nama, kelas, programStudi, dosen;
    int mhs;
};

struct Ruang {
    string nama;
    int kapasitas;
};

// ---- DATA: disalin persis dari penjadwalan.cpp (184 matkul, 13 ruang) ----
vector<MataKuliah> mk = {
        {"INF301A", "Metode Penelitian", "A", "S1-IF", "Dr. Nurjanah", 38},
        {"INF302A", "Jaringan Komputer", "A", "S1-IF", "Supriyanto", 40},
        {"INF303A", "Praktikum Jaringan Komputer", "A", "S1-IF", "Supriyanto", 40},
        {"INF304A", "Manajemen Proyek Perangkat Lunak", "A", "S1-IF", "Noor Falih", 36},
        {"INF305A", "Sistem Operasi", "A", "S1-IF", "Rido Zulfahmi", 37},
        {"INF306A", "Praktikum Sistem Operasi", "A", "S1-IF", "Rido Zulfahmi", 36},
        {"INF307A", "Kompleksitas Algoritma", "A", "S1-IF", "Ati Zaidiah", 40},
        {"INF308A", "Pemrograman Berorientasi Objek", "A", "S1-IF", "Bobby Suryo P.", 34},
        {"INF309A", "Praktikum PBO", "A", "S1-IF", "Bobby Suryo P.", 33},
        {"INF310A", "IMK", "A", "S1-IF", "Sarika", 35},

        {"INF301B", "Metode Penelitian", "B", "S1-IF", "Dr. Nurjanah", 32},
        {"INF302B", "Jaringan Komputer", "B", "S1-IF", "Supriyanto", 32},
        {"INF303B", "Praktikum Jaringan Komputer", "B", "S1-IF", "Supriyanto", 32},
        {"INF304B", "Manajemen Proyek Perangkat Lunak", "B", "S1-IF", "Noor Falih", 32},
        {"INF305B", "Sistem Operasi", "B", "S1-IF", "Rido Zulfahmi", 32},
        {"INF306B", "Praktikum Sistem Operasi", "B", "S1-IF", "Rido Zulfahmi", 32},
        {"INF307B", "Kompleksitas Algoritma", "B", "S1-IF", "Ati Zaidiah", 32},
        {"INF308B", "Pemrograman Berorientasi Objek", "B", "S1-IF", "Bobby Suryo P.", 32},
        {"INF309B", "Praktikum PBO", "B", "S1-IF", "Bobby Suryo P.", 32},
        {"INF310B", "IMK", "B", "S1-IF", "Sarika", 33},

        {"INF301C", "Metode Penelitian", "C", "S1-IF", "Dr. Nurjanah", 32},
        {"INF302C", "Jaringan Komputer", "C", "S1-IF", "Supriyanto", 32},
        {"INF303C", "Praktikum Jaringan Komputer", "C", "S1-IF", "Supriyanto", 33},
        {"INF304C", "Manajemen Proyek Perangkat Lunak", "C", "S1-IF", "Noor Falih", 32},
        {"INF305C", "Sistem Operasi", "C", "S1-IF", "Rido Zulfahmi", 32},
        {"INF306C", "Praktikum Sistem Operasi", "C", "S1-IF", "Rido Zulfahmi", 32},
        {"INF307C", "Kompleksitas Algoritma", "C", "S1-IF", "Ati Zaidiah", 33},
        {"INF308C", "Pemrograman Berorientasi Objek", "C", "S1-IF", "Bobby Suryo P.", 32},
        {"INF309C", "Praktikum PBO", "C", "S1-IF", "Bobby Suryo P.", 32},
        {"INF310C", "IMK", "C", "S1-IF", "Sarika", 32},

        {"INF301D", "Metode Penelitian", "D", "S1-IF", "Dr. Nurjanah", 34},
        {"INF302D", "Jaringan Komputer", "D", "S1-IF", "Supriyanto", 33},
        {"INF303D", "Praktikum Jaringan Komputer", "D", "S1-IF", "Supriyanto", 33},
        {"INF304D", "Manajemen Proyek Perangkat Lunak", "D", "S1-IF", "Noor Falih", 33},
        {"INF305D", "Sistem Operasi", "D", "S1-IF", "Rido Zulfahmi", 33},
        {"INF306D", "Praktikum Sistem Operasi", "D", "S1-IF", "Rido Zulfahmi", 33},
        {"INF307D", "Kompleksitas Algoritma", "D", "S1-IF", "Ati Zaidiah", 33},
        {"INF308D", "Pemrograman Berorientasi Objek", "D", "S1-IF", "Bobby Suryo P.", 33},
        {"INF309D", "Praktikum PBO", "D", "S1-IF", "Bobby Suryo P.", 33},
        {"INF310D", "IMK", "D", "S1-IF", "Sarika", 34},

        {"INF301E", "Metode Penelitian", "E", "S1-IF", "Dr. Nurjanah", 34},
        {"INF302E", "Jaringan Komputer", "E", "S1-IF", "Supriyanto", 33},
        {"INF303E", "Praktikum Jaringan Komputer", "E", "S1-IF", "Supriyanto", 34},
        {"INF304E", "Manajemen Proyek Perangkat Lunak", "E", "S1-IF", "Noor Falih", 33},
        {"INF305E", "Sistem Operasi", "E", "S1-IF", "Rido Zulfahmi", 34},
        {"INF306E", "Praktikum Sistem Operasi", "E", "S1-IF", "Rido Zulfahmi", 34},
        {"INF307E", "Kompleksitas Algoritma", "E", "S1-IF", "Ati Zaidiah", 35},
        {"INF308E", "Pemrograman Berorientasi Objek", "E", "S1-IF", "Bobby Suryo P.", 33},
        {"INF309E", "Praktikum PBO", "E", "S1-IF", "Bobby Suryo P.", 33},
        {"INF310E", "IMK", "E", "S1-IF", "Sarika", 33},

        {"INF301F", "Metode Penelitian", "F", "S1-IF", "Dr. Nurjanah", 34},
        {"INF302F", "Jaringan Komputer", "F", "S1-IF", "Supriyanto", 34},
        {"INF303F", "Praktikum Jaringan Komputer", "F", "S1-IF", "Supriyanto", 34},
        {"INF304F", "Manajemen Proyek Perangkat Lunak", "F", "S1-IF", "Noor Falih", 34},
        {"INF305F", "Sistem Operasi", "F", "S1-IF", "Didit Widiyanto", 34},
        {"INF306F", "Praktikum Sistem Operasi", "F", "S1-IF", "Didit Widiyanto", 34},
        {"INF307F", "Kompleksitas Algoritma", "F", "S1-IF", "Ati Zaidiah", 33},
        {"INF308F", "Pemrograman Berorientasi Objek", "F", "S1-IF", "Bobby Suryo P.", 34},
        {"INF309F", "Praktikum PBO", "F", "S1-IF", "Bobby Suryo P.", 34},
        {"INF310F", "IMK", "F", "S1-IF", "Sarika", 34},

        {"SIS301A", "Bahasa Inggris", "3A", "S1-SI", "Panji Alen S", 35},
        {"SIS302A", "Jaringan Komputer", "3A", "S1-SI", "Rizky Tito P.", 35},
        {"SIS303A", "Supply Chain Management", "3A", "S1-SI", "Erly Krisnanik", 35},
        {"SIS304A", "Manajemen Risiko SI", "3A", "S1-SI", "Erly Krisnanik", 35},
        {"SIS305A", "Ui/Ux Design", "3A", "S1-SI", "Sarika", 35},
        {"SIS306A", "Etika", "3A", "S1-SI", "Anita Muliawati", 35},
        {"SIS307A", "Pemrograman Web", "3A", "S1-SI", "Ruth Mariana", 35},
        {"SIS308A", "Prak. P Web", "3A", "S1-SI", "Ruth Mariana", 35},
        {"SIS309A", "Audit Sistem Informasi", "3A", "S1-SI", "Sarika", 35},
        {"SIS310A", "Sistem Informasi Manajemen", "3A", "S1-SI", "Erly Krisnanik", 35},
        {"SIS311A", "Transformasi Digital", "3A", "S1-SI", "Rifka Dwi A.", 35},
        {"SIS312A", "Sistem Operasi", "3A", "S1-SI", "Ruth Mariana", 35},
        {"SIS313A", "Prak. SO", "3A", "S1-SI", "Bobby Suryo P.", 35},

        {"SIS301B", "Bahasa Inggris", "3B", "S1-SI", "Aditya Chandra", 35},
        {"SIS302B", "Jaringan Komputer", "3B", "S1-SI", "Novi Trisman", 35},
        {"SIS303B", "Supply Chain Management", "3B", "S1-SI", "Mohamad Bayu", 35},
        {"SIS304B", "Manajemen Risiko SI", "3B", "S1-SI", "Sarika", 35},
        {"SIS305B", "Etika", "3B", "S1-SI", "Dr. Halim M.", 35},
        {"SIS306B", "Pemrograman Web", "3B", "S1-SI", "Catur Nugrahaeni", 35},
        {"SIS307B", "Prak. P Web", "3B", "S1-SI", "Ade Hikma T.", 35},
        {"SIS308B", "Audit Sistem Informasi", "3B", "S1-SI", "Sarika", 35},
        {"SIS309B", "Sistem Informasi Manajemen", "3B", "S1-SI", "Mohamad Bayu", 35},
        {"SIS310B", "Transformasi Digital", "3B", "S1-SI", "Hamonangan Kinantan", 35},
        {"SIS311B", "Sistem Operasi", "3B", "S1-SI", "Rido Zulfahmi", 35},
        {"SIS312B", "Prak. SO", "3B", "S1-SI", "Ruth Mariana", 35},

        {"SIS301C", "Bahasa Inggris", "3C", "S1-SI", "Panji Alen S", 35},
        {"SIS302C", "Jaringan Komputer", "3C", "S1-SI", "Hamonangan Kinantan", 35},
        {"SIS303C", "Etika", "3C", "S1-SI", "Sanggi Bayu Ardika", 35},
        {"SIS304C", "Pemrograman Web", "3C", "S1-SI", "Ade Hikma T.", 35},
        {"SIS305C", "Prak. P Web", "3C", "S1-SI", "Ade Hikma T.", 35},
        {"SIS306C", "Audit Sistem Informasi", "3C", "S1-SI", "Andhika Octa", 35},
        {"SIS307C", "Sistem Informasi Manajemen", "3C", "S1-SI", "Hamonangan Kinantan", 35},
        {"SIS308C", "Transformasi Digital", "3C", "S1-SI", "Andhika Octa", 35},
        {"SIS309C", "Sistem Operasi", "3C", "S1-SI", "Ruth Mariana", 35},
        {"SIS310C", "Prak. SO", "3C", "S1-SI", "Ruth Mariana", 35},

        {"SIS301D", "Bahasa Inggris", "3D", "S1-SI", "Rinawati Pelawi", 35},
        {"SIS302D", "Jaringan Komputer", "3D", "S1-SI", "Rido Zulfahmi", 35},
        {"SIS303D", "Etika", "3D", "S1-SI", "Novi Trisman", 35},
        {"SIS304D", "Pemrograman Web", "3D", "S1-SI", "Ruth Mariana", 35},
        {"SIS305D", "Prak. P Web", "3D", "S1-SI", "Ruth Mariana", 35},
        {"SIS306D", "Audit Sistem Informasi", "3D", "S1-SI", "Rifka Dwi A.", 35},
        {"SIS307D", "Sistem Informasi Manajemen", "3D", "S1-SI", "Novi Trisman", 35},
        {"SIS308D", "Transformasi Digital", "3D", "S1-SI", "Hamonangan Kinantan", 35},
        {"SIS309D", "Sistem Operasi", "3D", "S1-SI", "Bobby Suryo P.", 35},
        {"SIS310D", "Prak. SO", "3D", "S1-SI", "Bobby Suryo P.", 35},

        {"SIS301E", "Bahasa Inggris", "3E", "S1-SI", "Aditya Chandra", 35},
        {"SIS302E", "Jaringan Komputer", "3E", "S1-SI", "Rifka Dwi A.", 35},
        {"SIS303E", "Audit Sistem Informasi", "3E", "S1-SI", "Sarika", 35},
        {"SIS304E", "Pemrograman Web", "3E", "S1-SI", "Ruth Mariana", 35},
        {"SIS305E", "Prak. P Web", "3E", "S1-SI", "Ruth Mariana", 35},
        {"SIS306E", "Sistem Informasi Manajemen", "3E", "S1-SI", "Mohamad Bayu", 35},
        {"SIS307E", "Transformasi Digital", "3E", "S1-SI", "Rifka Dwi A.", 35},
        {"SIS308E", "Sistem Operasi", "3E", "S1-SI", "Ruth Mariana", 35},
        {"SIS309E", "Prak. SO", "3E", "S1-SI", "Bobby Suryo P.", 35},

        {"SIS301P", "Bahasa Inggris", "3PBI", "S1-SI", "Panji Alen S", 35},
        {"SIS302P", "Jaringan Komputer", "3PBI", "S1-SI", "Supriyanto", 35},
        {"SIS303P", "Ui/Ux Design", "3PBI", "S1-SI", "Sarika", 35},
        {"SIS304P", "Etika", "3PBI", "S1-SI", "Novi Trisman", 35},
        {"SIS305P", "Pemrograman Web", "3PBI", "S1-SI", "Ade Hikma T.", 35},
        {"SIS306P", "Prak. P Web", "3PBI", "S1-SI", "Ade Hikma T.", 35},
        {"SIS307P", "Audit Sistem Informasi", "3PBI", "S1-SI", "Erly Krisnanik", 35},
        {"SIS308P", "Sistem Informasi Manajemen", "3PBI", "S1-SI", "Ati Zaidiah", 35},
        {"SIS309P", "Transformasi Digital", "3PBI", "S1-SI", "Andhika Octa", 35},
        {"SIS310P", "Sistem Operasi", "3PBI", "S1-SI", "Bobby Suryo P.", 35},
        {"SIS311P", "Prak. SO", "3PBI", "S1-SI", "Rido Zulfahmi", 35},

        {"D3SI301A", "Metode Penulisan", "3A", "D3-SI", "Novi Trisman", 35},
        {"D3SI302A", "Sistem Operasi", "3A", "D3-SI", "Supriyanto", 35},
        {"D3SI303A", "Prak. Sistem Operasi", "3A", "D3-SI", "Supriyanto", 35},
        {"D3SI304A", "Pemrograman Web", "3A", "D3-SI", "Sanggi Bayu A.", 35},
        {"D3SI305A", "Prak. Pemrograman Web", "3A", "D3-SI", "Sanggi Bayu A.", 35},
        {"D3SI306A", "Statistik dan Probabilitas", "3A", "D3-SI", "Novi Trisman", 35},
        {"D3SI307A", "Technopreneurship", "3A", "D3-SI", "Sanggi Bayu A.", 35},
        {"D3SI308A", "Analisis Bisnis", "3A", "D3-SI", "Noor Falih", 35},
        {"D3SI309A", "IMK", "3A", "D3-SI", "Muhammad Panji M.", 35},
        {"D3SI310A", "Prak. IMK", "3A", "D3-SI", "Muhammad Panji M.", 35},
        {"D3SI311A", "Pengantar Data Science", "3A", "D3-SI", "Andhika Octa", 35},
        {"D3SI312A", "Manajemen Proyek SI", "3A", "D3-SI", "Bobby Suryo P.", 35},
        {"D3SI313A", "Prak. Dasar Pemrograman", "3A", "D3-SI", "Mohamad Bayu", 35},

        {"D3SI301B", "Metode Penulisan", "3B", "D3-SI", "Novi Trisman", 35},
        {"D3SI302B", "Sistem Operasi", "3B", "D3-SI", "Andhika Octa", 35},
        {"D3SI303B", "Prak. Sistem Operasi", "3B", "D3-SI", "Andhika Octa", 35},
        {"D3SI304B", "Pemrograman Web", "3B", "D3-SI", "Rizky Tito P.", 35},
        {"D3SI305B", "Prak. Pemrograman Web", "3B", "D3-SI", "Rizky Tito P.", 35},
        {"D3SI306B", "Statistik dan Probabilitas", "3B", "D3-SI", "Rido Zulfahmi", 35},
        {"D3SI307B", "Technopreneurship", "3B", "D3-SI", "Noor Falih", 35},
        {"D3SI308B", "Analisis Bisnis", "3B", "D3-SI", "Sarika", 35},
        {"D3SI309B", "IMK", "3B", "D3-SI", "Sarika", 35},
        {"D3SI310B", "Prak. IMK", "3B", "D3-SI", "Sarika", 35},
        {"D3SI311B", "Pengantar Data Science", "3B", "D3-SI", "Rido Zulfahmi", 35},
        {"D3SI312B", "Manajemen Proyek SI", "3B", "D3-SI", "Noor Falih", 35},
        {"D3SI313B", "Prak. Dasar Pemrograman", "3B", "D3-SI", "Anita Muliawati", 35},

        {"D3SI301C", "Metode Penulisan", "3C", "D3-SI", "Novi Trisman", 35},
        {"D3SI302C", "Sistem Operasi", "3C", "D3-SI", "Rido Zulfahmi", 35},
        {"D3SI303C", "Prak. Sistem Operasi", "3C", "D3-SI", "Rido Zulfahmi", 35},
        {"D3SI304C", "Pemrograman Web", "3C", "D3-SI", "Dr. Ridwan R.", 35},
        {"D3SI305C", "Prak. Pemrograman Web", "3C", "D3-SI", "Dr. Ridwan R.", 35},
        {"D3SI306C", "Statistik dan Probabilitas", "3C", "D3-SI", "Intan Novita D.", 35},
        {"D3SI307C", "Technopreneurship", "3C", "D3-SI", "Mohamad Bayu", 35},
        {"D3SI308C", "Analisis Bisnis", "3C", "D3-SI", "Novi Trisman", 35},
        {"D3SI309C", "IMK", "3C", "D3-SI", "Andhika Octa", 35},
        {"D3SI310C", "Prak. IMK", "3C", "D3-SI", "Andhika Octa", 35},
        {"D3SI311C", "Pengantar Data Science", "3C", "D3-SI", "Anita Muliawati", 35},
        {"D3SI312C", "Manajemen Proyek SI", "3C", "D3-SI", "Bobby Suryo P.", 35},
        {"D3SI313C", "Prak. Dasar Pemrograman", "3C", "D3-SI", "Rizky Tito P.", 35},

        {"D3SI301D", "Metode Penulisan", "3D", "D3-SI", "Novi Trisman", 35},
        {"D3SI302D", "Sistem Operasi", "3D", "D3-SI", "Bobby Suryo P.", 35},
        {"D3SI303D", "Prak. Sistem Operasi", "3D", "D3-SI", "Bobby Suryo P.", 35},
        {"D3SI304D", "Pemrograman Web", "3D", "D3-SI", "Andhika Octa", 35},
        {"D3SI305D", "Prak. Pemrograman Web", "3D", "D3-SI", "Andhika Octa", 35},
        {"D3SI306D", "Statistik dan Probabilitas", "3D", "D3-SI", "Intan Novita D.", 35},
        {"D3SI307D", "Technopreneurship", "3D", "D3-SI", "Noor Falih", 35},
        {"D3SI308D", "Analisis Bisnis", "3D", "D3-SI", "Novi Trisman", 35},
        {"D3SI309D", "IMK", "3D", "D3-SI", "Andhika Octa", 35},
        {"D3SI310D", "Prak. IMK", "3D", "D3-SI", "Andhika Octa", 35},
        {"D3SI311D", "Pengantar Data Science", "3D", "D3-SI", "Rido Zulfahmi", 35},
        {"D3SI312D", "Manajemen Proyek SI", "3D", "D3-SI", "Mohamad Bayu", 35},
        {"D3SI313D", "Prak. Dasar Pemrograman", "3D", "D3-SI", "Andhika Octa", 35},

        {"SDS301A", "IMK", "3A", "S1-SD", "Muhammad Panji M.", 35},
        {"SDS302A", "Analisis Big Data", "3A", "S1-SD", "Nindy Irzavika", 35},
        {"SDS303A", "Pembelajaran Mesin", "3A", "S1-SD", "Ruth Mariana", 35},
        {"SDS304A", "Pemrograman Web", "3A", "S1-SD", "Muhammad Panji M.", 35},
        {"SDS305A", "Basis Data Lanjut", "3A", "S1-SD", "Nindy Irzavika", 35},
        {"SDS306A", "Rekayasa Data", "3A", "S1-SD", "Anita Muliawati", 35},
        {"SDS307A", "Data Mining", "3A", "S1-SD", "Anita Muliawati", 35}
    };

vector<Ruang> rg = {
        {"FIK-201", 35}, {"FIK-202", 36}, {"FIK-203", 40},
        {"FIK-301", 40}, {"FIK-302", 40}, {"FIK-303", 40},
        {"FIKLAB-201", 35}, {"FIKLAB-301", 40}, {"FIKLAB-302", 35},
        {"FIKLAB-303", 40}, {"FIKLAB-304", 40}, {"FIKLAB-401", 40},
        {"VCR-FIK-KB-1", 50}
    };

const vector<string> HARI = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat"};
const vector<string> JAM  = {"07.10-08.50", "09.40-11.20", "13.00-14.40", "15.30-17.10"};

string labelSlot(int s) {
    return HARI[s / (int)JAM.size()] + " " + JAM[s % (int)JAM.size()];
}

// ---- Dinic (aliran maksimum) ----
struct Dinic {
    struct Edge { int to, rev, cap; };
    int N;
    vector<vector<Edge>> G;
    vector<int> level, it;
    long long bfsCount = 0, dfsVisits = 0;  // statistik untuk benchmark
    Dinic(int n) : N(n), G(n), level(n), it(n) {}
    void addEdge(int u, int v, int cap) {
        Edge a{v, (int)G[v].size(), cap};
        Edge b{u, (int)G[u].size(), 0};
        G[u].push_back(a);
        G[v].push_back(b);
    }
    bool bfs(int s, int t) {
        bfsCount++;
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto& e : G[u])
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
        }
        return level[t] >= 0;
    }
    int dfs(int u, int t, int f) {
        if (u == t) return f;
        for (int& i = it[u]; i < (int)G[u].size(); i++) {
            Edge& e = G[u][i];
            dfsVisits++;
            if (e.cap > 0 && level[e.to] == level[u] + 1) {
                int d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    G[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
    int maxflow(int s, int t) {
        int flow = 0;
        while (bfs(s, t)) {
            fill(it.begin(), it.end(), 0);
            int f;
            while ((f = dfs(s, t, INT_MAX)) > 0) flow += f;
        }
        return flow;
    }
};

struct HasilEfisien {
    vector<int> slot;   // per matkul
    vector<int> ruang;  // per matkul (indeks ke rg)
    int jumlahSlot = 0;
    int batasBawah = 0;
    int maksGrup = 0;
    long long bfsCount = 0, dfsVisits = 0;
};

HasilEfisien selesaikan(const vector<MataKuliah>& mk, const vector<Ruang>& rg) {
    int n = (int)mk.size();
    int R = (int)rg.size();

    // 1. Kelompok konflik (klik): satu (prodi, kelas).
    map<pair<string, string>, vector<int>> grup;
    for (int i = 0; i < n; i++)
        grup[{mk[i].programStudi, mk[i].kelas}].push_back(i);
    vector<vector<int>> glist;
    for (auto& kv : grup) glist.push_back(kv.second);
    int G = (int)glist.size();

    HasilEfisien H;
    H.slot.assign(n, -1);
    H.ruang.assign(n, -1);
    for (auto& g : glist) H.maksGrup = max(H.maksGrup, (int)g.size());
    H.batasBawah = max(H.maksGrup, (n + R - 1) / R);

    // Ruang diurutkan kapasitas menaik (untuk best-fit).
    vector<int> ruangUrut(R);
    iota(ruangUrut.begin(), ruangUrut.end(), 0);
    sort(ruangUrut.begin(), ruangUrut.end(),
         [&](int a, int b) { return rg[a].kapasitas < rg[b].kapasitas; });

    // 2. Cari S terkecil yang feasible, mulai dari batas bawah.
    const int TOTALSLOT = (int)(HARI.size() * JAM.size());  // 20
    for (int S = H.batasBawah; S <= TOTALSLOT; S++) {
        int SRC = 0, grup0 = 1, slot0 = 1 + G, SNK = 1 + G + S;
        Dinic dinic(SNK + 1);
        for (int gi = 0; gi < G; gi++)
            dinic.addEdge(SRC, grup0 + gi, (int)glist[gi].size());
        for (int gi = 0; gi < G; gi++)
            for (int s = 0; s < S; s++)
                dinic.addEdge(grup0 + gi, slot0 + s, 1);
        for (int s = 0; s < S; s++)
            dinic.addEdge(slot0 + s, SNK, R);
        if (dinic.maxflow(SRC, SNK) != n) continue;  // S belum cukup
        H.bfsCount = dinic.bfsCount;
        H.dfsVisits = dinic.dfsVisits;

        // 3. Ekstrak penempatan: edge grup->slot yang residual cap-nya 0
        //    berarti alirannya 1 (kelompok itu menaruh satu matkul di slot itu).
        vector<vector<int>> slotMatkul(S);
        for (int gi = 0; gi < G; gi++) {
            vector<int> slotTerpilih;
            for (auto& e : dinic.G[grup0 + gi])
                if (e.to >= slot0 && e.to < slot0 + S && e.cap == 0)
                    slotTerpilih.push_back(e.to - slot0);
            sort(slotTerpilih.begin(), slotTerpilih.end());
            for (size_t k = 0; k < glist[gi].size(); k++) {
                int idx = glist[gi][k];
                H.slot[idx] = slotTerpilih[k];
                slotMatkul[slotTerpilih[k]].push_back(idx);
            }
        }

        // 4. Tetapkan ruang best-fit per slot (matkul terbesar dulu).
        for (int s = 0; s < S; s++) {
            auto& v = slotMatkul[s];
            sort(v.begin(), v.end(),
                 [&](int a, int b) { return mk[a].mhs > mk[b].mhs; });
            vector<bool> pakai(R, false);
            for (int idx : v) {
                int pilih = -1;
                for (int r : ruangUrut)
                    if (!pakai[r] && rg[r].kapasitas >= mk[idx].mhs) {
                        pilih = r;
                        break;
                    }
                H.ruang[idx] = pilih;
                if (pilih != -1) pakai[pilih] = true;
            }
        }
        H.jumlahSlot = S;
        return H;
    }
    return H;  // tidak feasible sampai 20 slot
}

bool validasi(const vector<MataKuliah>& mk, const vector<Ruang>& rg,
              const HasilEfisien& H) {
    int n = (int)mk.size();
    map<int, vector<int>> perSlot;
    for (int i = 0; i < n; i++) {
        if (H.slot[i] < 0 || H.slot[i] >= H.jumlahSlot) return false;
        if (H.ruang[i] < 0 || H.ruang[i] >= (int)rg.size()) return false;
        if (rg[H.ruang[i]].kapasitas < mk[i].mhs) return false;
        perSlot[H.slot[i]].push_back(i);
    }
    for (auto& kv : perSlot) {
        set<pair<string, string>> grupLihat;
        set<int> ruangLihat;
        for (int i : kv.second) {
            if (!grupLihat.insert({mk[i].programStudi, mk[i].kelas}).second)
                return false;  // satu kelompok dobel di satu slot
            if (!ruangLihat.insert(H.ruang[i]).second)
                return false;  // satu ruang dobel di satu slot
        }
    }
    return true;
}

int main() {
    const int ULANG = 5000;

    HasilEfisien r = selesaikan(mk, rg);
    vector<int> isi(r.jumlahSlot, 0);
    for (int s : r.slot) isi[s]++;

    cout << "=== Penjadwalan EFISIEN (aliran maksimum + ruang best-fit) ===\n";
    set<pair<string, string>> grupHitung;
    for (auto& m : mk) grupHitung.insert({m.programStudi, m.kelas});
    cout << "Matkul: " << mk.size() << " | Ruang: " << rg.size()
         << " | Kelompok konflik: " << grupHitung.size() << "\n";
    cout << "Batas bawah slot: " << r.batasBawah
         << " (maks(klik terbesar " << r.maksGrup
         << ", ceil(" << mk.size() << "/" << rg.size() << ")="
         << (int)((mk.size() + rg.size() - 1) / rg.size()) << "))\n";
    cout << "Slot terpakai: " << r.jumlahSlot;
    if (r.jumlahSlot == r.batasBawah) cout << " (OPTIMAL, sama dengan batas bawah)";
    cout << "\n";
    if (r.jumlahSlot > 0) {
        int hariTerpakai = (r.jumlahSlot + (int)JAM.size() - 1) / (int)JAM.size();
        cout << "Hari terpakai: ";
        for (int d = 0; d < hariTerpakai; d++)
            cout << HARI[d] << (d + 1 == hariTerpakai ? "\n" : ", ");
    }
    cout << "Isi per slot: ";
    for (int i = 0; i < r.jumlahSlot; i++)
        cout << isi[i] << (i + 1 == r.jumlahSlot ? "\n" : " ");
    cout << "Validasi (konflik + ruang + kapasitas): "
         << (validasi(mk, rg, r) ? "LOLOS" : "GAGAL") << "\n";
    cout << "Statistik Dinic (1 run): BFS=" << r.bfsCount
         << ", kunjungan edge DFS=" << r.dfsVisits << "\n";

    cout << "\nJadwal final (urut slot, lalu ruang):\n";
    vector<int> idx(mk.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](int a, int b) {
        return r.slot[a] != r.slot[b] ? r.slot[a] < r.slot[b]
                                      : r.ruang[a] < r.ruang[b];
    });
    cout << left << setw(20) << "Waktu" << setw(10) << "Prodi" << setw(10) << "Kode"
         << setw(38) << "Mata Kuliah" << setw(8) << "Kelas" << setw(28)
         << "Dosen Pengampu" << "Ruang\n"
         << string(120, '-') << "\n";
    for (int i : idx)
        cout << left << setw(20) << labelSlot(r.slot[i]) << setw(10)
             << mk[i].programStudi << setw(10) << mk[i].kode << setw(38)
             << mk[i].nama << setw(8) << mk[i].kelas << setw(28) << mk[i].dosen
             << rg[r.ruang[i]].nama << "\n";

    long long sink = 0;
    auto t0 = chrono::steady_clock::now();
    for (int it = 0; it < ULANG; it++) {
        HasilEfisien h = selesaikan(mk, rg);
        for (int s : h.slot) sink += s;
        for (int x : h.ruang) sink += x;
    }
    auto t1 = chrono::steady_clock::now();
    double us = chrono::duration_cast<chrono::microseconds>(t1 - t0).count()
                / (double)ULANG;
    cout << "\nBenchmark (" << ULANG << " ulangan, fase solve saja):\n";
    cout << "  Rata-rata: " << fixed << setprecision(2) << us
         << " mikrodetik/run (checksum " << sink << ")\n";
    return 0;
}
