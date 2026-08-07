import marimo

__generated_with = "0.18.4"
app = marimo.App(width="medium")


@app.cell
def _():
    import marimo as mo
    return (mo,)


@app.cell
def _():
    for i in range(3, 9):
        print(i)
    return


@app.cell
def _(mo):
    _df = mo.sql(
        f"""
        CREATE OR REPLACE TABLE vector_bench AS
        SELECT
            unnest(benchmarks, recursive := true),
            2 AS 'procs',
            'Least Recently Used' AS strategy,
        FROM
            read_json('./bench_vector-strategy-0-procs-2.csv');
        """
    )
    return


@app.cell
def _():
    import duckdb

    strategies = {0: "Least Recently Used", 1: "Most Recently Used", 2: "Least Frequently Used", 3: "Pseudo-Least Recently Used"}

    for j in range(3, 7):
        for k, v in strategies.items():
        	duckdb.sql(f"""
                INSERT INTO vector_bench
                SELECT 
                    unnest(benchmarks, recursive := true),
                    {j} AS procs,
                    '{v}' AS strategy,
                FROM read_json('./bench_vector-strategy-{k}-procs-{j}.csv')
            """)
    return


@app.cell
def _(mo, vector_bench):
    _df = mo.sql(
        f"""
        COPY vector_bench TO 'strategy_processes_matrix_results.csv';
        """
    )
    return


@app.cell
def _(mo, vector_bench):
    bench_df = mo.sql(
        f"""
        SELECT
            REPLACE(STR_SPLIT(name, '/')[1], 'BM_', '') AS operation,
            STR_SPLIT(name, '/')[2] AS input_size,
            real_time / 1000 AS real_time_millis,
            procs,
            strategy
        FROM
            vector_bench
        WHERE strategy = 'Least Recently Used'
        AND operation NOT IN ('find_in_cache')
        ORDER BY procs, operation, input_size
        """
    )
    return (bench_df,)


@app.cell
def _(mo, vector_bench):
    bench_5proc_df = mo.sql(
        f"""
        SELECT
            REPLACE(STR_SPLIT(name, '/')[1], 'BM_', '') AS operation,
            STR_SPLIT(name, '/')[2] AS input_size,
            real_time / 1000 AS real_time_millis,
            procs,
            strategy
        FROM
            vector_bench
        WHERE operation NOT IN ('find_in_cache') AND procs = 5
        ORDER BY procs, operation, input_size
        """
    )
    return (bench_5proc_df,)


@app.cell
def _(mo, vector_bench):
    operation = mo.sql(
        f"""
        SELECT DISTINCT REPLACE(STR_SPLIT(name, '/')[1], 'BM_', '') FROM vector_bench WHERE name not like '%cache%'
        """
    )
    return


@app.cell
def _(alt, bench_df):
    _chart = (
    alt.Chart(bench_df)
    .mark_line(clip=True)
    .encode(
        x=alt.X(field='input_size', type='nominal', title='Input size (elements in vector)'),
        y=alt.Y(field='real_time_millis', type='quantitative', title='Time (ms)'),
        color=alt.Color(field='procs', type='nominal', title='MPI Processes'),
        column=alt.Column(field='operation', align='each', type='nominal', title='pc2l::Vector Function'),
        row=alt.Row(field='procs', align='each', title='MPI Processes'),
        tooltip=[
            alt.Tooltip(field='input_size'),
            alt.Tooltip(field='real_time', format=',.2f'),
            alt.Tooltip(field='procs')
        ]
    )

    .resolve_scale(y="independent")
    .properties(
        width=250,
        height=250,
        config={
            'axis': {
                'grid': False
            }
        }
        )
    )
    _chart.configure_header(
        titleColor='green',
        titleFontSize=14,
        labelColor='red',
        labelFontSize=14
    )
    _chart.save('vector_benchmarks_scalability.png')
    _chart
    return


@app.cell
def _(alt, bench_5proc_df):
    _chart = (
    # use only 5 processes for comparing across strategies
    alt.Chart(bench_5proc_df)
    .mark_line(clip=True)
    .encode(
        x=alt.X(field='input_size', type='nominal', title='Input size (elements in vector)'),
        y=alt.Y(field='real_time_millis', type='quantitative', title='Time (ms)'),
        color=alt.Color(field='strategy', type='nominal', title='Strategy'),
        column=alt.Column(field='operation', align='each', type='nominal', title='pc2l::Vector Function'),
        row=alt.Row(field='strategy', align='each', title='Strategy'),
        tooltip=[
            alt.Tooltip(field='input_size'),
            alt.Tooltip(field='real_time', format=',.2f'),
            alt.Tooltip(field='procs')
        ]
    )

    .resolve_scale(y="independent")
    .properties(
        width=250,
        height=250,
        config={
            'axis': {
                'grid': False
            }
        }
        )
    )
    _chart.configure_header(
        titleColor='green',
        titleFontSize=14,
        labelColor='red',
        labelFontSize=14
    )
    _chart.save('vector_benchmarks_by_strategy_5procs.png')
    _chart
    return


@app.cell
def _(bench_df):
    import altair as alt
    _chart = (
        alt.Chart(bench_df)
        .mark_line()
        .encode(
            x=alt.X(field='input_size', type='nominal', title='Input size (number of elements)'),
            y=alt.Y(field='real_time_millis', type='quantitative', title='Time (ms)'),
            color=alt.Color(field='operation', type='nominal', title='Operation'),
            tooltip=[
                alt.Tooltip(field='input_size'),
                alt.Tooltip(field='real_time', format=',.2f'),
                alt.Tooltip(field='operation')
            ]
        )
        .properties(
            height=500,
            width=500,
            config={
                'axis': {
                    'grid': False
                }
            }
        )
    )
    _chart
    return (alt,)


if __name__ == "__main__":
    app.run()
