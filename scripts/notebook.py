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
    return (i,)


@app.cell
def _(mo):
    _df = mo.sql(
        f"""
        CREATE OR REPLACE TABLE vector_bench AS
        SELECT
            unnest(benchmarks, recursive := true),
            2 AS 'procs'
        FROM
            read_json('./bench_vector_2-procs.csv');
        """
    )
    return


@app.cell
def _(i):
    import duckdb

    for j in range(3, 9):
    	duckdb.sql(f"""
            INSERT INTO vector_bench
            SELECT 
                unnest(benchmarks, recursive := true),
                {j} AS 'procs',
            FROM read_json('./bench_vector_{i}-procs.csv')
        """)
    return


@app.cell
def _(mo, vector_bench):
    bench_df = mo.sql(
        f"""
        SELECT
            REPLACE(STR_SPLIT(name, '/')[1], 'BM_', '') AS operationaaa,
            STR_SPLIT(name, '/')[2] AS input_size,
            ROUND(real_time / 1000, 2) AS real_time_millis,
            procs
        FROM
            vector_bench
        WHERE operation NOT IN ('find_in_cache')
        ORDER BY procs, operation, input_size
        """
    )
    return (bench_df,)


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
        y=alt.Y(field='real_time_millis', type='quantitative', title='Time (ms)').scale(domain=(0, 2000000)),
        color=alt.Color(field='procs', type='nominal', title='MPI Processes'),
        column=alt.Column(field='operation', align='each', type='nominal', title='pc2l::Vector Function'),
        row=alt.Row(field='procs', align='each', title='MPI Processes'),
        tooltip=[
            alt.Tooltip(field='input_size'),
            alt.Tooltip(field='real_time', format=',.2f'),
            alt.Tooltip(field='procs')
        ]
    )
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
