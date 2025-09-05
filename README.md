# Model Morana – symulacja i wizualizacja

Projekt zawiera implementację prostego procesu Morana z mutacjami kierującymi (driver) i pasażerskimi (passenger) oraz skrypt do wizualizacji wyników.

## Wymagania

- Kompilator C++ z obsługą C++17 (np. `g++`)
- Bash (do uruchomienia skryptu `run_model.sh`)
- Python 3 oraz pakiety: `pandas`, `matplotlib`

Instalacja pakietów Pythona:
```bash
python3 -m pip install --upgrade pip
python3 -m pip install pandas matplotlib
```

## Struktura projektu

- `moran_model.cpp` – implementacja modelu w C++ (generuje plik CSV z wynikami)
- `run_model.sh` – skrypt kompilujący i uruchamiający symulację
- `moran_plot.py` – skrypt generujący wykresy z plików CSV
- `outputs/` – katalog z wynikami symulacji (CSV)
- `plots/` – katalog z wykresami (PNG)

## Kompilacja i uruchomienie

Najprościej użyć skryptu:
```bash
bash run_model.sh
```
Skrypt:
- kompiluje `moran_model.cpp` z flagami `-O3 -Wall -Wextra`
- uruchamia symulację
- zapisuje wynik do `outputs/model_<…>.csv`

Jeśli chcesz kompilować ręcznie:
```bash
g++ -std=c++17 -O3 -Wall -Wextra -o moran_model moran_model.cpp
./moran_model A 100.0 50 0.15 0.35 10.0 0.2 outputs/custom.csv
```

## Parametry symulacji

Program C++ przyjmuje 8 argumentów:
```
<MODEL_TYPE: A|B> <tmax> <Ntot> <s> <d> <L> <p> <output_file>
```
- `MODEL_TYPE` – A: śmierć zależna od dopasowania; B: śmierć losowa
- `tmax` – maksymalny czas symulacji (double)
- `Ntot` – całkowita liczba komórek (int, populacja stała)
- `s` – przewaga fitnessu mutacji driver
- `d` – koszt fitnessu mutacji passenger
- `L` – całkowite tempo mutacji
- `p` – prawdopodobieństwo, że mutacja jest driverem (w przeciwnym razie passenger)
- `output_file` – ścieżka do pliku CSV z wynikami

Domyślne wartości w `run_model.sh` możesz zmienić, edytując sekcję parametrów:
```bash
MODEL_TYPE="B"
TMAX=100.0
NTOT=50
S=0.15
D=0.35
L=10.0
P=0.2
```

## Format danych wyjściowych (CSV)

Nagłówek:
```
Time,ActiveClones,CumulativeDrivers,CumulativePassengers
```
- `Time` – czas symulacji
- `ActiveClones` – liczba aktywnych klonów (n > 0)
- `CumulativeDrivers` – skumulowana liczba mutacji driver
- `CumulativePassengers` – skumulowana liczba mutacji passenger

## Generowanie wykresów

Po wykonaniu symulacji uruchom:
```bash
python3 moran_plot.py
```
Skrypt:
- czyta wszystkie pliki `*.csv` z katalogu `outputs/`
- generuje wykresy PNG do katalogu `plots/`:
  - `plot_ActiveClones.png`
  - `plot_CumulativeDrivers.png`
  - `plot_CumulativePassengers.png`

Etykiety serii są tworzone na podstawie nazwy pliku (np. `model_B_tmax_100.0_N_50_s_0.15_d_0.35.csv`).

## Ulepszenia i optymalizacje

- Naprawiono ostrzeżenia kompilatora (porównania signed/unsigned) przez użycie `std::size_t` w pętlach.
- Utwardzono skrypt `run_model.sh` (`set -euo pipefail`, bezpieczne cudzysłowy, flagi kompilacji).
- Wysokopoziomowe flagi optymalizacji (`-O3`) włączone domyślnie.

## Licencja

Zobacz `LICENSE`.