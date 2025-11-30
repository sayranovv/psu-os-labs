import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import rcParams

rcParams['font.sans-serif'] = ['DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

print("Загрузка данных...")

try:
    threads_df = pd.read_csv('threads_results.csv')
    processes_df = pd.read_csv('processes_results.csv')
    print(f"✓ Данные загружены")
    print(f"  - Потоки: {len(threads_df)} измерений")
    print(f"  - Процессы: {len(processes_df)} измерений")
except FileNotFoundError as e:
    print(f"✗ Ошибка: файл не найден - {e}")
    print("Убедитесь, что запустили build_and_test.sh")
    exit(1)

print("\nДанные потоков:")
print(threads_df)
print("\nДанные процессов:")
print(processes_df)

def calculate_speedup(df):
    speedups = []
    base_time = df[df['num_threads' if 'num_threads' in df.columns else 'num_processes'] == 1]['average_time_ms'].values[0]
    
    for _, row in df.iterrows():
        time_col = 'average_time_ms'
        speedup = base_time / row[time_col]
        speedups.append(speedup)
    
    df['speedup'] = speedups
    return df

threads_df = calculate_speedup(threads_df)
processes_df = calculate_speedup(processes_df)

threads_df = threads_df.rename(columns={'num_threads': 'num_units'})
processes_df = processes_df.rename(columns={'num_processes': 'num_units'})

print("\nДанные потоков со Speedup:")
print(threads_df)
print("\nДанные процессов со Speedup:")
print(processes_df)

fig = plt.figure(figsize=(15, 12))

ax1 = plt.subplot(2, 3, 1)
ax1.plot(threads_df['num_units'], threads_df['average_time_ms'], 
         marker='o', linewidth=2, markersize=8, color='blue', label='Потоки')
ax1.set_xlabel('Количество потоков', fontsize=11)
ax1.set_ylabel('Время выполнения (мс)', fontsize=11)
ax1.set_title('Время выполнения - Потоки', fontsize=12, fontweight='bold')
ax1.grid(True, alpha=0.3)
ax1.legend()

ax2 = plt.subplot(2, 3, 2)
ax2.plot(processes_df['num_units'], processes_df['average_time_ms'], 
         marker='s', linewidth=2, markersize=8, color='red', label='Процессы')
ax2.set_xlabel('Количество процессов', fontsize=11)
ax2.set_ylabel('Время выполнения (мс)', fontsize=11)
ax2.set_title('Время выполнения - Процессы', fontsize=12, fontweight='bold')
ax2.grid(True, alpha=0.3)
ax2.legend()

ax3 = plt.subplot(2, 3, 3)
common_units = sorted(set(threads_df['num_units']).intersection(set(processes_df['num_units'])))
threads_times = threads_df[threads_df['num_units'].isin(common_units)]['average_time_ms'].values
processes_times = processes_df[processes_df['num_units'].isin(common_units)]['average_time_ms'].values

x = np.arange(len(common_units))
width = 0.35

ax3.bar(x - width/2, threads_times, width, label='Потоки', color='blue', alpha=0.8)
ax3.bar(x + width/2, processes_times, width, label='Процессы', color='red', alpha=0.8)
ax3.set_xlabel('Количество потоков/процессов', fontsize=11)
ax3.set_ylabel('Время выполнения (мс)', fontsize=11)
ax3.set_title('Сравнение: Потоки vs Процессы', fontsize=12, fontweight='bold')
ax3.set_xticks(x)
ax3.set_xticklabels(common_units)
ax3.legend()
ax3.grid(True, alpha=0.3, axis='y')

ax4 = plt.subplot(2, 3, 4)

ideal_speedup = threads_df['num_units'].values
ax4.plot(threads_df['num_units'], ideal_speedup, 
         linewidth=2, color='green', linestyle='--', label='Идеальное (линейное)')

ax4.plot(threads_df['num_units'], threads_df['speedup'],
         marker='o', linewidth=2, markersize=8, color='blue', label='Реальное')

ax4.set_xlabel('Количество потоков', fontsize=11)
ax4.set_ylabel('Ускорение (Speedup)', fontsize=11)
ax4.set_title('Ускорение - Потоки', fontsize=12, fontweight='bold')
ax4.legend()
ax4.grid(True, alpha=0.3)

ax5 = plt.subplot(2, 3, 5)

ideal_speedup_proc = processes_df['num_units'].values
ax5.plot(processes_df['num_units'], ideal_speedup_proc, 
         linewidth=2, color='green', linestyle='--', label='Идеальное (линейное)')

ax5.plot(processes_df['num_units'], processes_df['speedup'], 
         marker='s', linewidth=2, markersize=8, color='red', label='Реальное')

ax5.set_xlabel('Количество процессов', fontsize=11)
ax5.set_ylabel('Ускорение (Speedup)', fontsize=11)
ax5.set_title('Ускорение - Процессы', fontsize=12, fontweight='bold')
ax5.legend()
ax5.grid(True, alpha=0.3)

ax6 = plt.subplot(2, 3, 6)

common_speedups_threads = threads_df[threads_df['num_units'].isin(common_units)]['speedup'].values
common_speedups_processes = processes_df[processes_df['num_units'].isin(common_units)]['speedup'].values

x = np.arange(len(common_units))
width = 0.35

ax6.bar(x - width/2, common_speedups_threads, width, label='Потоки', color='blue', alpha=0.8)
ax6.bar(x + width/2, common_speedups_processes, width, label='Процессы', color='red', alpha=0.8)

ax6.plot(x, [common_units[i] for i in range(len(common_units))],
         linewidth=2, color='green', linestyle='--', marker='x', label='Идеальное')

ax6.set_xlabel('Количество потоков/процессов', fontsize=11)
ax6.set_ylabel('Ускорение (Speedup)', fontsize=11)
ax6.set_title('Сравнение ускорения', fontsize=12, fontweight='bold')
ax6.set_xticks(x)
ax6.set_xticklabels(common_units)
ax6.legend()
ax6.grid(True, alpha=0.3, axis='y')

plt.tight_layout()
plt.savefig('performance_analysis.png', dpi=150, bbox_inches='tight')
print("\n✓ График сохранён как 'performance_analysis.png'")

plt.show()

print("\n" + "="*60)
print("СТАТИСТИКА")
print("="*60)

print("\nПОТОКИ:")
print(f"  Минимальное время: {threads_df['average_time_ms'].min():.2f} мс")
print(f"  Максимальное время: {threads_df['average_time_ms'].max():.2f} мс")
print(f"  Максимальное ускорение: {threads_df['speedup'].max():.2f}x")
print(f"  Количество потоков при максимальном ускорении: {threads_df.loc[threads_df['speedup'].idxmax(), 'num_units']}")

print("\nПРОЦЕССЫ:")
print(f"  Минимальное время: {processes_df['average_time_ms'].min():.2f} мс")
print(f"  Максимальное время: {processes_df['average_time_ms'].max():.2f} мс")
print(f"  Максимальное ускорение: {processes_df['speedup'].max():.2f}x")
print(f"  Количество процессов при максимальном ускорении: {processes_df.loc[processes_df['speedup'].idxmax(), 'num_units']}")

print("\nСРАВНЕНИЕ:")
if len(common_units) > 0:
    fastest_single = min(
        threads_df[threads_df['num_units'] == 1]['average_time_ms'].values[0],
        processes_df[processes_df['num_units'] == 1]['average_time_ms'].values[0]
    )
    slowest_single = max(
        threads_df[threads_df['num_units'] == 1]['average_time_ms'].values[0],
        processes_df[processes_df['num_units'] == 1]['average_time_ms'].values[0]
    )
    
    overhead = ((slowest_single - fastest_single) / fastest_single) * 100
    
    if threads_df[threads_df['num_units'] == 1]['average_time_ms'].values[0] < processes_df[processes_df['num_units'] == 1]['average_time_ms'].values[0]:
        print(f"  Потоки на {overhead:.1f}% быстрее, чем процессы")
    else:
        print(f"  Процессы на {overhead:.1f}% быстрее, чем потоки")

print("\n" + "="*60)
