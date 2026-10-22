import pandas as pd

df_table = pd.read_csv('./strategy_processes_matrix_results.csv')
with open('latex_table.tex', 'w') as f:
    f.write(df_table.to_latex(index=False))

