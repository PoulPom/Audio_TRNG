import csv
import matplotlib.pyplot as plt
import numpy as np 

def calculate_entropy(data):
    if not data:
        return 0.0
    value_counts = {}
    for value in data:
        if value in value_counts:
            value_counts[value] += 1
        else:
            value_counts[value] = 1
    probabilities = np.array([count / len(data) for count in value_counts.values()])
    entropy = -np.sum(probabilities * np.log2(probabilities + 1e-10))  # Adding a small value to avoid log(0)
    return entropy

def plot_histogram(data, title, min_value, max_value):
    plt.figure(figsize=(12, 6))
    bins = np.arange(min_value, max_value + 2, 8)
    weights = np.ones_like(data, dtype=float) / len(data)
    
    plt.hist(data, bins=bins, weights=weights, color='blue', edgecolor='black', alpha=0.8)
    plt.title(title, fontsize=14)
    plt.ylabel('Prawdopodobieństwo', fontsize=12)
    plt.ylim(0, 1.0)
    plt.xlim(min_value, max_value)
    
    plt.show()

def main():
    try:
        with open('build/entropy_data.csv', 'r') as file_raw, open('build/processed_entropy_data.csv', 'r') as file_processed:
            reader_raw = csv.reader(file_raw)
            reader_processed = csv.reader(file_processed)
            raw_data = [int(row[0]) for row in reader_raw if row]
            processed_data = [int(row[0]) for row in reader_processed if row]
            
    except FileNotFoundError:
        print("Nie można znaleźć plików :c.")
        return
    except ValueError as e:
        print(f"Błąd konwersji danych: {e}")
        return
    entropy_raw = calculate_entropy(raw_data)
    entropy_processed = calculate_entropy(processed_data)
    title_raw = f'Histogram surowych danych (Entropia: {entropy_raw:.5f})'
    title_processed = f'Histogram po procesie hiperhaosu (Entropia: {entropy_processed:.5f})'
    print(f"Entropia źródła:                 {entropy_raw:.2f}")
    print(f"Entropia po procesie hiperhaosu: {entropy_processed:.2f}")
    plot_histogram(raw_data, title_raw, 0, 255)
    plot_histogram(processed_data, title_processed, 0, 255)

if __name__ == "__main__":
    main()