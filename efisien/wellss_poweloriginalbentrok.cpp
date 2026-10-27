// ============================================================================
// welsh_powell_deteksi.cpp
// Menjalankan Welsh-Powell Original + Simulasi Jadwal Fisik + Deteksi Bentrok
// ============================================================================

#include <bits/stdc++.h>
using namespace std;

struct MataKuliah { string kode, nama, kelas, programStudi, dosen; int mhs; };
struct Ruang { string nama; int kapasitas; };
struct JadwalEntry { int idMatkul, slot, ruang; };

vector<MataKuliah> mk = {
    // 184 Data Mata Kuliah
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
    if (s < 0 || s >= 20) return "Slot Berlebih";
    return HARI[s / (int)JAM.size()] + " " + JAM[s % (int)JAM.size()];
}

void deteksiBentrok(const vector<JadwalEntry>& jadwal) {
    int totalBentrokGrup = 0, totalBentrokRuang = 0;
    map<pair<int, int>, vector<int>> pemakaianRuang; 
    map<pair<int, string>, vector<int>> jadwalMahasiswa; 

    for (const auto& j : jadwal) {
        pemakaianRuang[{j.slot, j.ruang}].push_back(j.idMatkul);
        string grupKey = mk[j.idMatkul].programStudi + "_" + mk[j.idMatkul].kelas;
        jadwalMahasiswa[{j.slot, grupKey}].push_back(j.idMatkul);
    }

    cout << "\n=================================================================================\n";
    cout << " HASIL DETEKSI BENTROK (Welsh-Powell Original)\n";
    cout << "=================================================================================\n";
    for (auto& kv : pemakaianRuang) {
        if (kv.second.size() > 1) {
            totalBentrokRuang++;
            if (totalBentrokRuang <= 5) {
                cout << "[BENTROK RUANG] " << labelSlot(kv.first.first) << " @ " << rg[kv.first.second].nama 
                     << " diperebutkan oleh " << kv.second.size() << " mata kuliah.\n";
            }
        }
    }
    if (totalBentrokRuang > 5) cout << "... (dan " << totalBentrokRuang - 5 << " kasus bentrok ruang lainnya disembunyikan)\n";

    for (auto& kv : jadwalMahasiswa) {
        if (kv.second.size() > 1) {
            totalBentrokGrup++;
        }
    }
    
    cout << "-> RINGKASAN: " << totalBentrokRuang << " Bentrok Ruang | " << totalBentrokGrup << " Bentrok Mahasiswa\n";
    cout << "-> STATUS: " << ((totalBentrokRuang == 0 && totalBentrokGrup == 0) ? "LOLOS (JADWAL VALID)" : "GAGAL (TERJADI BENTROK)") << "\n\n";
}

struct HasilWP { vector<int> warna; int jumlahWarna; };

HasilWP welshPowellOriginal(const vector<MataKuliah>& mk) {
    int n = mk.size();
    vector<vector<char>> adj(n, vector<char>(n, 0));
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (mk[i].kelas == mk[j].kelas && mk[i].programStudi == mk[j].programStudi)
                adj[i][j] = adj[j][i] = 1;

    HasilWP H; H.warna.assign(n, -1);
    vector<int> derajat(n, 0), urut(n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) derajat[i] += adj[i][j];
    iota(urut.begin(), urut.end(), 0);
    stable_sort(urut.begin(), urut.end(), [&](int a, int b) { return derajat[a] > derajat[b]; });

    int sisa = n, warna = 0;
    while (sisa > 0) {
        vector<int> anggota;
        for (int v : urut) {
            if (H.warna[v] != -1) continue;
            bool bentrok = false;
            for (int u : anggota) if (adj[v][u]) { bentrok = true; break; }
            if (bentrok) continue;
            H.warna[v] = warna; anggota.push_back(v); sisa--;
        }
        warna++;
    }
    H.jumlahWarna = warna; return H;
}

int main() {
    HasilWP rWP = welshPowellOriginal(mk);
    
    // PEMETAAN WARNA KE JADWAL FISIK (SIMULASI BENTROK)
    vector<JadwalEntry> jadwalWP;
    map<int, int> ruangCounterPerWarna;
    for (size_t i = 0; i < mk.size(); i++) {
        int slotWarna = rWP.warna[i];
        int ruangFisik = ruangCounterPerWarna[slotWarna] % rg.size(); // Paksa masuk ruang (Penyebab Bentrok)
        ruangCounterPerWarna[slotWarna]++;
        jadwalWP.push_back({(int)i, slotWarna, ruangFisik});
    }
    
    deteksiBentrok(jadwalWP);

    // BENCHMARK 5000 RUNS
    const int ULANG = 5000;
    long long sink = 0;
    auto t0 = chrono::steady_clock::now();
    for (int it = 0; it < ULANG; it++) {
        HasilWP h = welshPowellOriginal(mk);
        for (int w : h.warna) sink += w;
    }
    auto t1 = chrono::steady_clock::now();
    double us = chrono::duration_cast<chrono::microseconds>(t1 - t0).count() / (double)ULANG;
    
    cout << "=== BENCHMARK (5000 Iterasi) ===\n";
    cout << "Kecepatan Welsh-Powell Original: " << fixed << setprecision(2) << us << " mikrodetik/run\n";
    return 0;
}
