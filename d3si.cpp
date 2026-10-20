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
    string kode, nama, kelas, programStudi; 
    int mhs; 
};

struct Ruang { 
    string nama; 
    int kapasitas; 
};

struct Hasil { 
    int slot = -1; 
    int ruang = -1; 
};

const vector<string> HARI = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat"};
const vector<string> JAM  = {"07.10-08.50", "09.40-11.20", "13.00-14.40", "15.30-17.10"};
const int TOTALSLOT = (int)(HARI.size() * JAM.size());

string labelSlot(int s) { 
    return HARI[s / JAM.size()] + " " + JAM[s % JAM.size()]; 
}

struct Jadwal { 
    vector<Hasil> h; 
    int warna = 0; 
    vector<int> urut, derajat; 
};

vector<vector<bool>> bangunGraf(const vector<MataKuliah>& mk) {
    int n = (int)mk.size();
    vector<vector<bool>> adj(n, vector<bool>(n, false));
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (mk[i].kelas == mk[j].kelas && mk[i].programStudi == mk[j].programStudi) {
                adj[i][j] = adj[j][i] = true;
            }
        }
    }
    return adj;
}

int memoCariRuang(int kapasitasMhs, const vector<Ruang>& rg, vector<int>& memo) {
    if (kapasitasMhs < 0) return -1;
    if (memo[kapasitasMhs] != -2) return memo[kapasitasMhs];

    int r_opt = -1;
    for (int k = 0; k < (int)rg.size(); k++) {
        if (rg[k].kapasitas >= kapasitasMhs) {
            if (r_opt == -1 || rg[k].kapasitas < rg[r_opt].kapasitas) {
                r_opt = k;
            }
        }
    }
    return memo[kapasitasMhs] = r_opt;
}

Jadwal welshPowell(const vector<MataKuliah>& mk, const vector<Ruang>& rg) {
    int n = (int)mk.size();
    auto adj = bangunGraf(mk);
    Jadwal J; 
    J.h.assign(n, Hasil()); 
    J.derajat.assign(n, 0);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) 
            J.derajat[i] += adj[i][j];

    J.urut.resize(n);
    iota(J.urut.begin(), J.urut.end(), 0);
    
    stable_sort(J.urut.begin(), J.urut.end(), [&](int a, int b) { 
        return J.derajat[a] > J.derajat[b]; 
    });

    int max_kap = 0;
    for(auto& r : rg) max_kap = max(max_kap, r.kapasitas);
    vector<int> memo(max_kap + 1, -2);

    int sisa = n;
    while (sisa > 0) {
        vector<bool> dipakai(rg.size(), false);
        vector<int> anggota;
        for (int v : J.urut) {
            if (J.h[v].slot != -1) continue;
            bool bentrok = false;
            for (int u : anggota) {
                if (adj[v][u]) { 
                    bentrok = true; 
                    break; 
                }
            }
            if (bentrok) continue;

            int r = memoCariRuang(mk[v].mhs, rg, memo);
            if (r == -1 || dipakai[r]) {
                r = -1;
                for (int k = 0; k < (int)rg.size(); k++) {
                    if (!dipakai[k] && rg[k].kapasitas >= mk[v].mhs) {
                        if (r == -1 || rg[k].kapasitas < rg[r].kapasitas) r = k;
                    }
                }
            }
            if (r == -1) continue;

            J.h[v] = {J.warna, r};
            dipakai[r] = true; 
            anggota.push_back(v); 
            sisa--;
        }
        if (anggota.empty()) throw runtime_error("Kapasitas slot waktu atau ruang mingguan tidak mencukupi");
        J.warna++;
    }
    return J;
}

bool validasi(const vector<MataKuliah>& mk, const vector<Ruang>& rg, const Jadwal& J) {
    auto adj = bangunGraf(mk);
    int n = (int)mk.size();
    for (int i = 0; i < n; i++) {
        if (rg[J.h[i].ruang].kapasitas < mk[i].mhs) return false;
        for (int j = i + 1; j < n; j++)
            if (J.h[i].slot == J.h[j].slot && (adj[i][j] || J.h[i].ruang == J.h[j].ruang))
                return false;
    }
    return true;
}

void cetakJadwal(const vector<MataKuliah>& mk, const vector<Ruang>& rg, const Jadwal& J) {
    if (mk.empty()) { cout << "(Belum ada mata kuliah)\n"; return; }
    vector<int> idx(mk.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](int a, int b) {
        return J.h[a].slot != J.h[b].slot ? J.h[a].slot < J.h[b].slot : J.h[a].ruang < J.h[b].ruang;
    });
    cout << left << setw(20) << "Waktu" << setw(10) << "Prodi" << setw(10) << "Kode" << setw(42) << "Mata Kuliah" 
         << setw(8) << "Kelas" << "Ruang\n" << string(98, '-') << "\n";
    for (int i : idx) {
        cout << left << setw(20) << labelSlot(J.h[i].slot) 
             << setw(10) << mk[i].programStudi 
             << setw(10) << mk[i].kode 
             << setw(42) << mk[i].nama 
             << setw(8)  << mk[i].kelas 
             << rg[J.h[i].ruang].nama << "\n";
    }
    cout << "\nTotal Sesi: " << mk.size() << " | Slot Terpakai: " << J.warna << "/" << TOTALSLOT 
         << " | Validasi Konflik: " << (validasi(mk, rg, J) ? "LOLOS (Tanpa Bentrok)" : "GAGAL") << "\n";
}

void simpanCSV(const string& file, const vector<MataKuliah>& mk, const vector<Ruang>& rg, const Jadwal& J) {
    ofstream out(file);
    out << "waktu;prodi;kode;matakuliah;kelas;ruang\n";
    for (size_t i = 0; i < mk.size(); i++)
        out << labelSlot(J.h[i].slot) << ';' << mk[i].programStudi << ';' << mk[i].kode << ';' 
            << mk[i].nama << ';' << mk[i].kelas << ';' << rg[J.h[i].ruang].nama << '\n';
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

void cetakPerubahan(const vector<MataKuliah>& mk, const vector<Ruang>& rg, 
                    const map<string, Hasil>& lama, const Jadwal& J) {
    bool ada = false;
    for (size_t i = 0; i < mk.size(); i++) {
        auto it = lama.find(mk[i].kode);
        if (it == lama.end()) {
            cout << "  [BARU]   " << mk[i].kode << " (" << mk[i].kelas << ") -> " << labelSlot(J.h[i].slot) 
                 << ", " << rg[J.h[i].ruang].nama << "\n"; ada = true;
        } else if (it->second.slot != J.h[i].slot || it->second.ruang != J.h[i].ruang) {
            cout << "  [PINDAH] " << mk[i].kode << " (" << mk[i].kelas << "): " << labelSlot(it->second.slot) << " -> " 
                 << labelSlot(J.h[i].slot) << ", " << rg[J.h[i].ruang].nama << "\n"; ada = true;
        }
    }
    if (!ada) cout << "  (Tidak ada perubahan jadwal)\n";
}

int kapasitasMaks(const vector<Ruang>& rg) {
    int m = 0; for (auto& r : rg) m = max(m, r.kapasitas); return m;
}

int tanyaMenu(const string& label) {
    while (true) {
        string s = tanya(label);
        try { size_t p; int v = stoi(s, &p); if (p == s.size() && v >= 0 && v <= 5) return v; } catch (...) {}
        cout << "  Pilihan tidak valid.\n";
    }
}

bool susunUlang(vector<MataKuliah>& mk, vector<Ruang>& rg, Jadwal& J, 
                const vector<MataKuliah>& mkLama, const vector<Ruang>& rgLama) {
    try {
        Jadwal baru = welshPowell(mk, rg);
        if (baru.warna > TOTALSLOT) throw runtime_error("Slot waktu mingguan tidak cukup");
        J = baru; return true;
    } catch (const exception& e) {
        cout << "  GAGAL: " << e.what() << ". Data dikembalikan seperti semula.\n";
        mk = mkLama; rg = rgLama; return false;
    }
}

int main() {
    vector<MataKuliah> mk = {
        {"INF301A", "Metode Penelitian", "A", "S1-IF", 38},
        {"INF302A", "Jaringan Komputer", "A", "S1-IF", 40},
        {"INF303A", "Praktikum Jaringan Komputer", "A", "S1-IF", 40},
        {"INF304A", "Manajemen Proyek Perangkat Lunak", "A", "S1-IF", 36},
        {"INF305A", "Sistem Operasi", "A", "S1-IF", 37},
        {"INF306A", "Praktikum Sistem Operasi", "A", "S1-IF", 36},
        {"INF307A", "Kompleksitas Algoritma", "A", "S1-IF", 40},
        {"INF308A", "Pemrograman Berorientasi Objek", "A", "S1-IF", 34},
        {"INF309A", "Praktikum Pemrograman Berorientasi Objek", "A", "S1-IF", 33},
        {"INF310A", "Interaksi Manusia dan Komputer", "A", "S1-IF", 35},

        {"INF301B", "Metode Penelitian", "B", "S1-IF", 32},
        {"INF302B", "Jaringan Komputer", "B", "S1-IF", 32},
        {"INF303B", "Praktikum Jaringan Komputer", "B", "S1-IF", 32},
        {"INF304B", "Manajemen Proyek Perangkat Lunak", "B", "S1-IF", 32},
        {"INF305B", "Sistem Operasi", "B", "S1-IF", 32},
        {"INF306B", "Praktikum Sistem Operasi", "B", "S1-IF", 32},
        {"INF307B", "Kompleksitas Algoritma", "B", "S1-IF", 32},
        {"INF308B", "Pemrograman Berorientasi Objek", "B", "S1-IF", 32},
        {"INF309B", "Praktikum Pemrograman Berorientasi Objek", "B", "S1-IF", 32},
        {"INF310B", "Interaksi Manusia dan Komputer", "B", "S1-IF", 33},

        {"INF301C", "Metode Penelitian", "C", "S1-IF", 32},
        {"INF302C", "Jaringan Komputer", "C", "S1-IF", 32},
        {"INF303C", "Praktikum Jaringan Komputer", "C", "S1-IF", 33},
        {"INF304C", "Manajemen Proyek Perangkat Lunak", "C", "S1-IF", 32},
        {"INF305C", "Sistem Operasi", "C", "S1-IF", 32},
        {"INF306C", "Praktikum Sistem Operasi", "C", "S1-IF", 32},
        {"INF307C", "Kompleksitas Algoritma", "C", "S1-IF", 33},
        {"INF308C", "Pemrograman Berorientasi Objek", "C", "S1-IF", 32},
        {"INF309C", "Praktikum Pemrograman Berorientasi Objek", "C", "S1-IF", 32},
        {"INF310C", "Interaksi Manusia dan Komputer", "C", "S1-IF", 32},

        {"INF301D", "Metode Penelitian", "D", "S1-IF", 34},
        {"INF302D", "Jaringan Komputer", "D", "S1-IF", 33},
        {"INF303D", "Praktikum Jaringan Komputer", "D", "S1-IF", 33},
        {"INF304D", "Manajemen Proyek Perangkat Lunak", "D", "S1-IF", 33},
        {"INF305D", "Sistem Operasi", "D", "S1-IF", 33},
        {"INF306D", "Praktikum Sistem Operasi", "D", "S1-IF", 33},
        {"INF307D", "Kompleksitas Algoritma", "D", "S1-IF", 33},
        {"INF308D", "Pemrograman Berorientasi Objek", "D", "S1-IF", 33},
        {"INF309D", "Praktikum Pemrograman Berorientasi Objek", "D", "S1-IF", 33},
        {"INF310D", "Interaksi Manusia dan Komputer", "D", "S1-IF", 34},

        {"INF301E", "Metode Penelitian", "E", "S1-IF", 34},
        {"INF302E", "Jaringan Komputer", "E", "S1-IF", 33},
        {"INF303E", "Praktikum Jaringan Komputer", "E", "S1-IF", 34},
        {"INF304E", "Manajemen Proyek Perangkat Lunak", "E", "S1-IF", 33},
        {"INF305E", "Sistem Operasi", "E", "S1-IF", 34},
        {"INF306E", "Praktikum Sistem Operasi", "E", "S1-IF", 34},
        {"INF307E", "Kompleksitas Algoritma", "E", "S1-IF", 35},
        {"INF308E", "Pemrograman Berorientasi Objek", "E", "S1-IF", 33},
        {"INF309E", "Praktikum Pemrograman Berorientasi Objek", "E", "S1-IF", 33},
        {"INF310E", "Interaksi Manusia dan Komputer", "E", "S1-IF", 33},

        {"INF301F", "Metode Penelitian", "F", "S1-IF", 34},
        {"INF302F", "Jaringan Komputer", "F", "S1-IF", 34},
        {"INF303F", "Praktikum Jaringan Komputer", "F", "S1-IF", 34},
        {"INF304F", "Manajemen Proyek Perangkat Lunak", "F", "S1-IF", 34},
        {"INF305F", "Sistem Operasi", "F", "S1-IF", 34},
        {"INF306F", "Praktikum Sistem Operasi", "F", "S1-IF", 34},
        {"INF307F", "Kompleksitas Algoritma", "F", "S1-IF", 33},
        {"INF308F", "Pemrograman Berorientasi Objek", "F", "S1-IF", 34},
        {"INF309F", "Praktikum Pemrograman Berorientasi Objek", "F", "S1-IF", 34},
        {"INF310F", "Interaksi Manusia dan Komputer", "F", "S1-IF", 34},

        {"D3SI301A", "Metode Penulisan", "3A", "D3-SI", 35},
        {"D3SI302A", "Sistem Operasi", "3A", "D3-SI", 35},
        {"D3SI303A", "Prak. Sistem Operasi", "3A", "D3-SI", 35},
        {"D3SI304A", "Pemrograman Web", "3A", "D3-SI", 35},
        {"D3SI305A", "Prak. Pemrograman Web", "3A", "D3-SI", 35},
        {"D3SI306A", "Statistik dan Probabilitas", "3A", "D3-SI", 35},
        {"D3SI307A", "Technopreneurship", "3A", "D3-SI", 35},
        {"D3SI308A", "Analisis Bisnis", "3A", "D3-SI", 35},
        {"D3SI309A", "IMK", "3A", "D3-SI", 35},
        {"D3SI310A", "Prak. IMK", "3A", "D3-SI", 35},
        {"D3SI311A", "Pengantar Data Science", "3A", "D3-SI", 35},
        {"D3SI312A", "Manajemen Proyek Sistem Informasi", "3A", "D3-SI", 35},
        {"D3SI313A", "Prak. Dasar Pemrograman", "3A", "D3-SI", 35},

        {"D3SI301B", "Metode Penulisan", "3B", "D3-SI", 35},
        {"D3SI302B", "Sistem Operasi", "3B", "D3-SI", 35},
        {"D3SI303B", "Prak. Sistem Operasi", "3B", "D3-SI", 35},
        {"D3SI304B", "Pemrograman Web", "3B", "D3-SI", 35},
        {"D3SI305B", "Prak. Pemrograman Web", "3B", "D3-SI", 35},
        {"D3SI306B", "Statistik dan Probabilitas", "3B", "D3-SI", 35},
        {"D3SI307B", "Technopreneurship", "3B", "D3-SI", 35},
        {"D3SI308B", "Analisis Bisnis", "3B", "D3-SI", 35},
        {"D3SI309B", "IMK", "3B", "D3-SI", 35},
        {"D3SI310B", "Prak. IMK", "3B", "D3-SI", 35},
        {"D3SI311B", "Pengantar Data Science", "3B", "D3-SI", 35},
        {"D3SI312B", "Manajemen Proyek Sistem Informasi", "3B", "D3-SI", 35},
        {"D3SI313B", "Prak. Dasar Pemrograman", "3B", "D3-SI", 35},

        {"D3SI301C", "Metode Penulisan", "3C", "D3-SI", 35},
        {"D3SI302C", "Sistem Operasi", "3C", "D3-SI", 35},
        {"D3SI303C", "Prak. Sistem Operasi", "3C", "D3-SI", 35},
        {"D3SI304C", "Pemrograman Web", "3C", "D3-SI", 35},
        {"D3SI305C", "Prak. Pemrograman Web", "3C", "D3-SI", 35},
        {"D3SI306C", "Statistik dan Probabilitas", "3C", "D3-SI", 35},
        {"D3SI307C", "Technopreneurship", "3C", "D3-SI", 35},
        {"D3SI308C", "Analisis Bisnis", "3C", "D3-SI", 35},
        {"D3SI309C", "IMK", "3C", "D3-SI", 35},
        {"D3SI310C", "Prak. IMK", "3C", "D3-SI", 35},
        {"D3SI311C", "Pengantar Data Science", "3C", "D3-SI", 35},
        {"D3SI312C", "Manajemen Proyek Sistem Informasi", "3C", "D3-SI", 35},
        {"D3SI313C", "Prak. Dasar Pemrograman", "3C", "D3-SI", 35},

        {"D3SI301D", "Metode Penulisan", "3D", "D3-SI", 35},
        {"D3SI302D", "Sistem Operasi", "3D", "D3-SI", 35},
        {"D3SI303D", "Prak. Sistem Operasi", "3D", "D3-SI", 35},
        {"D3SI304D", "Pemrograman Web", "3D", "D3-SI", 35},
        {"D3SI305D", "Prak. Pemrograman Web", "3D", "D3-SI", 35},
        {"D3SI306D", "Statistik dan Probabilitas", "3D", "D3-SI", 35},
        {"D3SI307D", "Technopreneurship", "3D", "D3-SI", 35},
        {"D3SI308D", "Analisis Bisnis", "3D", "D3-SI", 35},
        {"D3SI309D", "IMK", "3D", "D3-SI", 35},
        {"D3SI310D", "Prak. IMK", "3D", "D3-SI", 35},
        {"D3SI311D", "Pengantar Data Science", "3D", "D3-SI", 35},
        {"D3SI312D", "Manajemen Proyek Sistem Informasi", "3D", "D3-SI", 35},
        {"D3SI313D", "Prak. Dasar Pemrograman", "3D", "D3-SI", 35}
    };

    vector<Ruang> rg = {
        {"FIK-201", 35}, {"FIK-202", 36}, {"FIK-203", 40},
        {"FIK-301", 40}, {"FIK-302", 40}, {"FIK-303", 40},
        {"FIKLAB-201", 35}, {"FIKLAB-301", 40}, {"FIKLAB-302", 35},
        {"FIKLAB-303", 40}, {"FIKLAB-304", 40}, {"FIKLAB-401", 40},
        {"VCR-FIK-KB-1", 50}
    };

    Jadwal J = welshPowell(mk, rg);
    cout << "=== PENJADWALAN OTOMATIS S1 IF & D3 SI SEMESTER 3 BERHASIL DIBUAT ===\n";
    cetakJadwal(mk, rg, J);

    while (true) {
        cout << "\n===== MENU UTAMA PENJADWALAN INTERAKTIF =====\n"
             << "1. Tampilkan Jadwal\n"
             << "2. Tambah Mata Kuliah Baru\n"
             << "3. Hapus Mata Kuliah\n"
             << "4. Tambah Ruangan Baru\n"
             << "5. Simpan Jadwal ke CSV (jadwal_sem3.csv)\n"
             << "0. Keluar\n";
        int pilih = tanyaMenu("Pilih menu: ");
        if (pilih == 0) break;

        auto mkLama = mk; auto rgLama = rg;
        map<string, Hasil> lama;
        for (size_t i = 0; i < mk.size(); i++) lama[mk[i].kode] = J.h[i];

        if (pilih == 1) {
            cetakJadwal(mk, rg, J);
        } else if (pilih == 2) {
            MataKuliah m;
            m.programStudi = tanya("Program Studi (cth: S1-IF / D3-SI): ");
            m.kode = tanya("Kode MK       : ");
            for (auto& x : mk) if (x.kode == m.kode) { cout << "  Kode sudah terdaftar.\n"; m.kode.clear(); break; }
            if (m.kode.empty()) continue;
            m.nama  = tanya("Nama MK       : ");
            m.kelas = tanya("Kelas         : ");
            m.mhs   = tanyaAngka("Jml Mahasiswa : ");
            if (m.mhs > kapasitasMaks(rg)) {
                cout << "  Ditolak: Mahasiswa melebihi kapasitas ruang terbesar (" << kapasitasMaks(rg) << ").\n"; continue;
            }
            mk.push_back(m);
            if (susunUlang(mk, rg, J, mkLama, rgLama)) {
                cout << "\nBerhasil ditambahkan, jadwal disusun ulang:\n";
                cetakPerubahan(mk, rg, lama, J);
                cetakJadwal(mk, rg, J);
            }
        } else if (pilih == 3) {
            string kode = tanya("Kode MK yang dihapus: ");
            auto it = find_if(mk.begin(), mk.end(), [&](const MataKuliah& x) { return x.kode == kode; });
            if (it == mk.end()) { cout << "  Kode tidak ditemukan.\n"; continue; }
            mk.erase(it);
            if (mk.empty()) { J = Jadwal(); cout << "  Semua mata kuliah kosong.\n"; continue; }
            if (susunUlang(mk, rg, J, mkLama, rgLama)) {
                cout << "\nBerhasil dihapus, jadwal disusun ulang:\n";
                cetakPerubahan(mk, rg, lama, J);
                cetakJadwal(mk, rg, J);
            }
        } else if (pilih == 4) {
            Ruang r;
            r.nama = tanya("Nama Ruang    : ");
            r.kapasitas = tanyaAngka("Kapasitas     : ");
            rg.push_back(r);
            if (susunUlang(mk, rg, J, mkLama, rgLama)) {
                cout << "\nRuangan baru ditambahkan, jadwal disusun ulang:\n";
                cetakPerubahan(mk, rg, lama, J);
                cetakJadwal(mk, rg, J);
            }
        } else if (pilih == 5) {
            simpanCSV("jadwal_sem3.csv", mk, rg, J);
            cout << "  Jadwal berhasil disimpan ke file 'jadwal_sem3.csv'\n";
        }
    }
    cout << "Program selesai.\n";
    return 0;
}
