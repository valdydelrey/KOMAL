Berikut adalah program C++17 yang sudah diperbarui dengan memasukkan **seluruh jadwal Kelas A sampai F** beserta 3 mata kuliah peminatan:

```cpp
// =====================================================================
// Penjadwalan Kuliah Interaktif - Semester 3 (Kelas A - F) | C++17
// Kompilasi : g++ -std=c++17 -O2 penjadwalan_semua_kelas.cpp -o jadwal_semua
// Jalankan  : ./jadwal_semua
// =====================================================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <map>
#include <fstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

struct MataKuliah { 
    string hari, kode, nama, kelas, waktu, ruangan; 
    int sks; 
};

struct Ruang { 
    string nama; 
    int kapasitas; 
};

struct Hasil { 
    int slot = -1; 
    int ruang = -1; 
};

const vector<string> HARI_LIST = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat"};

struct Jadwal { 
    vector<Hasil> h; 
    int warna = 0; 
    vector<int> urut, derajat; 
};

void cetakJadwal(const vector<MataKuliah>& mk, const string& filterKelas = "SEMUA") {
    if (mk.empty()) { cout << "(Belum ada mata kuliah)\n"; return; }
    
    cout << left << setw(10) << "Hari" 
         << setw(10) << "Kode"
         << setw(42) << "Mata Kuliah" 
         << setw(8)  << "Kelas" 
         << setw(6)  << "SKS" 
         << setw(16) << "Waktu" 
         << setw(15) << "Ruangan\n" 
         << string(107, '-') << "\n";

    int totalSesi = 0;
    for (const auto& m : mk) {
        if (filterKelas != "SEMUA" && m.kelas != filterKelas) continue;
        
        string ruangDisplay = m.ruangan.empty() ? "-" : m.ruangan;
        cout << left << setw(10) << m.hari 
             << setw(10) << m.kode
             << setw(42) << m.nama 
             << setw(8)  << m.kelas 
             << setw(6)  << m.sks 
             << setw(16) << m.waktu 
             << setw(15) << ruangDisplay << "\n";
        totalSesi++;
    }
    cout << "\nTotal Sesi Ditampilkan: " << totalSesi << "\n";
}

void simpanCSV(const string& file, const vector<MataKuliah>& mk) {
    ofstream out(file);
    out << "hari;kode;matakuliah;kelas;sks;waktu;ruangan\n";
    for (const auto& m : mk) {
        out << m.hari << ';' << m.kode << ';' << m.nama << ';' 
            << m.kelas << ';' << m.sks << ';' << m.waktu << ';' 
            << m.ruangan << '\n';
    }
}

string tanya(const string& label) {
    string s;
    do { cout << label; if (!getline(cin, s)) exit(0); } while (s.empty());
    return s;
}

int tanyaAngka(const string& label) {
    while (true) {
        string s = tanya(label);
        try { size_t p; int v = stoi(s, &p); if (p == s.size() && v > 0) return v; } catch (...) {}
        cout << "  Masukkan bilangan bulat positif.\n";
    }
}

int tanyaMenu(const string& label) {
    while (true) {
        string s = tanya(label);
        try { size_t p; int v = stoi(s, &p); if (p == s.size() && v >= 0 && v <= 5) return v; } catch (...) {}
        cout << "  Pilihan tidak valid.\n";
    }
}

int main() {
    // Dataset Seluruh Kelas (A - F & Peminatan) Semester 3
    vector<MataKuliah> mk = {
        // === KELAS A ===
        {"Senin",  "INF301", "Metode Penelitian", "A", "13:00-15:30", "FIK-301", 3},
        {"Selasa", "INF302", "Interaksi Manusia dan Komputer", "A", "07:10-09:40", "FIK-201", 3},
        {"Selasa", "INF303", "Sistem Operasi", "A", "09:40-11:20", "FIKLAB-301", 2},
        {"Selasa", "INF304", "Praktikum Sistem Operasi", "A", "11:20-12:10", "FIKLAB-301", 1},
        {"Rabu",   "INF305", "Manajemen Proyek Perangkat Lunak", "A", "07:10-09:40", "FIKLAB-304", 3},
        {"Rabu",   "INF306", "Kompleksitas Algoritma", "A", "09:40-12:10", "FIK-303", 3},
        {"Kamis",  "INF307", "Jaringan Komputer", "A", "07:10-08:50", "FIKLAB-301", 2},
        {"Kamis",  "INF308", "Praktikum Jaringan Komputer", "A", "08:50-09:40", "FIKLAB-301", 1},
        {"Kamis",  "INF309", "Interaksi Manusia dan Komputer (Sesi 2)", "A", "15:30-18:00", "VCR-KB-1", 3},
        {"Jumat",  "INF310", "Pemrograman Berorientasi Objek", "A", "08:00-09:40", "FIKLAB-201", 2},
        {"Jumat",  "INF311", "Praktikum PBO", "A", "09:40-10:30", "FIKLAB-201", 1},
        {"Jumat",  "INF312", "Pemrograman Berorientasi Objek (Sesi 2)", "A", "13:30-15:10", "VCR", 2},

        // === KELAS B ===
        {"Senin",  "INF313", "Metode Penelitian", "B", "13:00-15:30", "FIK-302", 3},
        {"Senin",  "INF314", "Manajemen Proyek Perangkat Lunak", "B", "15:30-18:00", "FIKLAB-302", 3},
        {"Selasa", "INF315", "Sistem Operasi", "B", "09:40-11:20", "FIKLAB-302", 2},
        {"Selasa", "INF316", "Praktikum Sistem Operasi", "B", "11:20-12:10", "FIKLAB-302", 1},
        {"Rabu",   "INF317", "Kompleksitas Algoritma", "B", "09:40-12:10", "FIK-302", 3},
        {"Rabu",   "INF318", "Interaksi Manusia dan Komputer", "B", "13:00-15:30", "VCR-KK", 3},
        {"Kamis",  "INF319", "Jaringan Komputer", "B", "07:10-08:50", "FIKLAB-303", 2},
        {"Kamis",  "INF320", "Praktikum Jaringan Komputer", "B", "08:50-09:40", "FIKLAB-303", 1},
        {"Kamis",  "INF321", "Interaksi Manusia dan Komputer (Sesi 2)", "B", "15:30-18:00", "VCR-KB-1", 3},
        {"Jumat",  "INF322", "Pemrograman Berorientasi Objek", "B", "08:00-09:40", "FIKLAB-302", 2},
        {"Jumat",  "INF323", "Praktikum PBO", "B", "09:40-10:30", "FIKLAB-302", 1},
        {"Jumat",  "INF324", "Pemrograman Berorientasi Objek (Sesi 2)", "B", "13:30-15:10", "VCR", 2},

        // === KELAS C ===
        {"Senin",  "INF325", "Interaksi Manusia dan Komputer", "C", "09:40-12:10", "FIKLAB-303", 3},
        {"Senin",  "INF326", "Metode Penelitian", "C", "13:00-15:30", "FIK-303", 3},
        {"Selasa", "INF327", "Sistem Operasi", "C", "09:40-11:20", "FIKLAB-303", 2},
        {"Selasa", "INF328", "Praktikum Sistem Operasi", "C", "11:20-12:10", "FIKLAB-303", 1},
        {"Rabu",   "INF329", "Manajemen Proyek Perangkat Lunak", "C", "07:10-09:40", "FIKLAB-401", 3},
        {"Rabu",   "INF330", "Kompleksitas Algoritma", "C", "09:40-12:10", "FIK-401", 3},
        {"Rabu",   "INF331", "Interaksi Manusia dan Komputer (Sesi 2)", "C", "13:00-15:30", "VCR-KK", 3},
        {"Kamis",  "INF332", "Jaringan Komputer", "C", "07:10-08:50", "FIKLAB-201", 2},
        {"Kamis",  "INF333", "Praktikum Jaringan Komputer", "C", "08:50-09:40", "FIKLAB-201", 1},
        {"Kamis",  "INF334", "Interaksi Manusia dan Komputer (Sesi 3)", "C", "15:30-18:00", "VCR-KB-1", 3},
        {"Jumat",  "INF335", "Pemrograman Berorientasi Objek", "C", "08:00-09:40", "FIKLAB-301", 2},
        {"Jumat",  "INF336", "Praktikum PBO", "C", "09:40-10:30", "FIKLAB-301", 1},
        {"Jumat",  "INF337", "Pemrograman Berorientasi Objek (Sesi 2)", "C", "13:30-15:10", "VCR", 2},

        // === KELAS D ===
        {"Senin",  "INF338", "Metode Penelitian", "D", "15:30-18:00", "FIK-202", 3},
        {"Selasa", "INF339", "Sistem Operasi", "D", "07:10-08:50", "FIKLAB-301/VCR", 2},
        {"Selasa", "INF340", "Praktikum Sistem Operasi", "D", "08:50-09:40", "FIKLAB-301", 1},
        {"Rabu",   "INF341", "Kompleksitas Algoritma", "D", "13:00-15:30", "FIK-301", 3},
        {"Kamis",  "INF342", "Interaksi Manusia dan Komputer", "D", "07:10-09:40", "FIKLAB-401", 3},
        {"Kamis",  "INF343", "Jaringan Komputer", "D", "09:40-11:20", "FIKLAB-301", 2},
        {"Kamis",  "INF344", "Praktikum Jaringan Komputer", "D", "11:20-12:10", "FIKLAB-301", 1},
        {"Kamis",  "INF345", "Interaksi Manusia dan Komputer (Sesi 2)", "D", "15:30-18:00", "VCR-KB-1", 3},
        {"Jumat",  "INF346", "Manajemen Proyek Perangkat Lunak", "D", "08:00-10:30", "FIKLAB-304", 3},
        {"Jumat",  "INF347", "Pemrograman Berorientasi Objek", "D", "13:30-15:10", "FIKLAB-201", 2},
        {"Jumat",  "INF348", "Praktikum PBO", "D", "15:10-16:00", "FIKLAB-201", 1},

        // === KELAS E ===
        {"Senin",  "INF349", "Interaksi Manusia dan Komputer", "E", "07:10-09:40", "FIKLAB-303", 3},
        {"Senin",  "INF350", "Manajemen Proyek Perangkat Lunak", "E", "13:00-15:30", "FIKLAB-301", 3},
        {"Senin",  "INF351", "Metode Penelitian", "E", "15:30-18:00", "FIKLAB-202", 3},
        {"Selasa", "INF352", "Sistem Operasi", "E", "07:10-08:50", "FIKLAB-302/VCR", 2},
        {"Selasa", "INF353", "Praktikum Sistem Operasi", "E", "08:50-09:40", "FIKLAB-302", 1},
        {"Rabu",   "INF354", "Interaksi Manusia dan Komputer (Sesi 2)", "E", "09:40-12:10", "VCR-KK", 3},
        {"Rabu",   "INF355", "Kompleksitas Algoritma", "E", "13:00-15:30", "FIK-203", 3},
        {"Kamis",  "INF356", "Jaringan Komputer", "E", "09:40-11:20", "FIKLAB-303", 2},
        {"Kamis",  "INF357", "Praktikum Jaringan Komputer", "E", "11:20-12:10", "FIKLAB-303", 1},
        {"Kamis",  "INF358", "Interaksi Manusia dan Komputer (Sesi 3)", "E", "15:30-18:00", "VCR-KB-1", 3},
        {"Jumat",  "INF359", "Pemrograman Berorientasi Objek", "E", "13:30-15:10", "FIKLAB-302", 2},
        {"Jumat",  "INF360", "Praktikum PBO", "E", "15:10-16:00", "FIKLAB-302", 1},

        // === KELAS F ===
        {"Senin",  "INF361", "Interaksi Manusia dan Komputer", "F", "07:10-09:40", "FIKLAB-304", 3},
        {"Senin",  "INF362", "Metode Penelitian", "F", "15:30-18:00", "FIKLAB-203", 3},
        {"Selasa", "INF363", "Sistem Operasi", "F", "07:10-08:50", "FIKLAB-303/VCR", 2},
        {"Selasa", "INF364", "Praktikum Sistem Operasi", "F", "08:50-09:40", "FIKLAB-303", 1},
        {"Selasa", "INF365", "Interaksi Manusia dan Komputer (Sesi 2)", "F", "09:40-12:10", "VCR-KK", 3},
        {"Rabu",   "INF366", "Kompleksitas Algoritma", "F", "13:00-15:30", "FIK-302", 3},
        {"Kamis",  "INF367", "Jaringan Komputer", "F", "13:00-14:40", "FIKLAB-201", 2},
        {"Kamis",  "INF368", "Praktikum Jaringan Komputer", "F", "14:40-15:30", "FIKLAB-201", 1},
        {"Kamis",  "INF369", "Interaksi Manusia dan Komputer (Sesi 3)", "F", "15:30-18:00", "VCR-KB-1", 3},
        {"Jumat",  "INF370", "Manajemen Proyek Perangkat Lunak", "F", "08:00-10:30", "FIKLAB-401", 3},
        {"Jumat",  "INF371", "Pemrograman Berorientasi Objek", "F", "13:30-15:10", "FIKLAB-301", 2},
        {"Jumat",  "INF372", "Praktikum PBO", "F", "15:10-16:00", "FIKLAB-301", 1},

        // === PEMINATAN ===
        {"Senin",  "PEM301", "VR/AR SE.1", "Peminatan C", "15:30-18:00", "FIKLAB-303", 3},
        {"Selasa", "PEM302", "Keamanan Siber CE.1", "Peminatan A", "13:00-15:30", "FIKLAB-202", 3},
        {"Selasa", "PEM303", "Keamanan Siber CE.1", "Peminatan B", "13:00-15:30", "FIK-201", 3},
        {"Selasa", "PEM304", "Komputasi Awan NE.1", "Peminatan A", "15:30-18:00", "FIK-201", 3},
        {"Kamis",  "PEM305", "VR/AR SE.1", "Peminatan A", "15:30-18:00", "FIKLAB-303", 3},
        {"Kamis",  "PEM306", "VR/AR SE.1", "Peminatan B", "15:30-18:00", "FIKLAB-402", 3}
    };

    cout << "=== JADWAL KULIAH SEMESTER 3 (KELAS A - F) ===\n\n";

    while (true) {
        cout << "\n===== MENU INTERAKTIF JADWAL SEMESTER 3 =====\n"
             << "1. Tampilkan Semua Jadwal (Kelas A - F & Peminatan)\n"
             << "2. Filter Jadwal berdasarkan Kelas (A, B, C, D, E, F, atau Peminatan)\n"
             << "3. Tambah Mata Kuliah Baru\n"
             << "4. Hapus Mata Kuliah berdasarkan Kode\n"
             << "5. Simpan Jadwal ke CSV (jadwal_semua_kelas.csv)\n"
             << "0. Keluar\n";
        int pilih = tanyaMenu("Pilih menu: ");
        if (pilih == 0) break;

        if (pilih == 1) {
            cetakJadwal(mk, "SEMUA");
        } else if (pilih == 2) {
            string k = tanya("Masukkan nama Kelas (A/B/C/D/E/F/Peminatan A/dll): ");
            cetakJadwal(mk, k);
        } else if (pilih == 3) {
            MataKuliah m;
            m.hari    = tanya("Hari (Contoh: Senin) : ");
            m.kode    = tanya("Kode MK              : ");
            m.nama    = tanya("Nama Mata Kuliah     : ");
            m.kelas   = tanya("Kelas (A/B/C/dll)    : ");
            m.sks     = tanyaAngka("Jumlah SKS           : ");
            m.waktu   = tanya("Waktu (cth 08:00-09:40): ");
            m.ruangan = tanya("Ruangan              : ");
            
            mk.push_back(m);
            cout << "\n  Mata kuliah berhasil ditambahkan!\n";
        } else if (pilih == 4) {
            string kode = tanya("Masukkan Kode MK yang ingin dihapus: ");
            auto it = remove_if(mk.begin(), mk.end(), [&](const MataKuliah& x) { return x.kode == kode; });
            if (it == mk.end()) {
                cout << "  Kode mata kuliah tidak ditemukan.\n";
            } else {
                mk.erase(it, mk.end());
                cout << "  Mata kuliah berhasil dihapus.\n";
            }
        } else if (pilih == 5) {
            simpanCSV("jadwal_semua_kelas.csv", mk);
            cout << "  Jadwal berhasil disimpan ke file 'jadwal_semua_kelas.csv'\n";
        }
    }
    cout << "Program selesai.\n";
    return 0;
}

```
