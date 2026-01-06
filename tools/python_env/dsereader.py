#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math
import os
import time

import datetime
import matplotlib.pyplot as plt
import pandas as pd
import psutil
import seaborn as sns

sns.set_theme()

application_names = {
    "bipartite": {"name": "bipartite"},
    "fig8": {"name": "fig8"},
    "modem": {"name": "modem"},
    "sample": {"name": "sample"},
    "satellite": {"name": "satellite"},
    "samplerate": {"name": "samplerate"},
    "BlackScholes": {"name": "BlackScholes"},
    "example": {"name": "example"},
    "Echo": {"name": "Echo"},
    "PDectect": {"name": "PDectect"},
    "H264": {"name": "H264"},
    "h263decoder": {"name": "h263decoder"},
    "JPEG2000": {"name": "JPEG2000"},
    "buffercycle": {"name": "buffercycle"},
}

method_names = {
    #  0 Infos
    #  1 Throughput
    "kdse": {"name": "KDSE", "color": "black"},
    "k2dse": {"name": "K2DSE", "color": "black"},
    "k2dsea": {"name": "K2DSEA", "color": "black"},
    "k2dseC": {"name": "K2DSE w/ cache", "color": "black"},
    "k2dseaC": {"name": "K2DSEA w/ cache", "color": "black"},

    "kdseP": {"name": "KDSE C 24", "color": "black"},
    "k2dseCP": {"name": "K2DSE C 24", "color": "black"},
    "k2dseaCP": {"name": "K2DSEA  C 24", "color": "black"},

    #    8 : { "name" : "KDSE2" ,     "color" : "black"}, # "-athroughputbufferingDSE -prealtime=1 -pmode=KDSE -pthread=2"
    #    8 : { "name" : "KDSE4" ,     "color" : "black"}, # "-athroughputbufferingDSE -prealtime=1 -pmode=KDSE -pthread=4"
    #   10 : { "name" : "KDSE8" ,     "color" : "black"}, # "-athroughputbufferingDSE -prealtime=1 -pmode=KDSE -pthread=8"
    #    9 : { "name" : "KDSE16" ,    "color" : "black"}, # "-athroughputbufferingDSE -prealtime=1 -pmode=KDSE -pthread=16"
}


def time_in_msec(
        time_msec):  # copy pasted from https://stackoverflow.com/questions/48063828/convert-duration-format-from-float-to-monthdayshoursminutesseconds-in-python
    time_sec = int(time_msec // 1000)
    delta = datetime.timedelta(seconds=time_sec)
    delta_str = str(delta)[-8:]
    hours, minutes, seconds = [int(val) for val in delta_str.split(":", 3)]
    days = delta.days % 7
    return "{}days {}h {}min {}.{}sec ({})".format(days, hours, minutes, seconds, int(time_msec) & 1000, time_msec)


def extract_throughput(throughput_file):
    for line in open(throughput_file).read().split("\n"):
        if 'KPeriodic Throughput is' in line:
            th = float(line.split(" ")[-1])
            return th
    return None


def extract_task_count(infos_file):
    for line in open(infos_file).read().split("\n"):
        if 'Task count' in line:
            count = int(line.split(" ")[-1])
            return count
    return None


def extract_logs(log_dir):
    global application_names, method_names

    logs = {}

    # Collect all the filename
    for f in glob.glob(f"{log_dir}/*.txt"):
        name = f.split("/")[-1].split(".")[0]
        method = name.split("_")[1]
        application = name.split("_")[0]
        if application not in logs:
            logs[application] = {'logs': {}}

        logs[application]["logs"][method] = f

    # Clean incomplete apps
    delete_them = set()
    for app in logs:
        if not "throughput" in logs[app]["logs"]:
            delete_them.add(app)
        if not "infos" in logs[app]["logs"]:
            delete_them.add(app)
    for app in delete_them:
        del logs[app]

    # Check missing apps amd methods
    for app in logs:
        if app not in application_names:
            print("No such application", app)
            logs[app]["name"] = app
            application_names[app] = {"name": app}
        else:
            logs[app]["name"] = application_names[app]["name"]

        for method in logs[app]["logs"]:
            if method not in method_names and method not in ["throughput", "infos"]:
                print("No such method", method)
                method_names[method] = {"name": method, "color": "black"}

    # Load task count and throughput
    for app in logs:
        logs[app]["max_throughput"] = extract_throughput(logs[app]["logs"]["throughput"])
        logs[app]["task_count"] = extract_task_count(logs[app]["logs"]["infos"])
        del logs[app]["logs"]["throughput"]
        del logs[app]["logs"]["infos"]

    return logs


def gen_dse_data(infos, columns=None):
    if columns is None:
        columns = ["throughput",
                   "storage distribution size",
                   "cumulative duration"]
    list_of_dict = []

    for app, app_infos in infos.items():
        if not "max_throughput" in app_infos or not "task_count" in app_infos:
            print("No such application", app)
            continue
        app_name = app_infos["name"]
        app_max_throughput = app_infos["max_throughput"]
        app_task_count = app_infos["task_count"]
        for m in app_infos["logs"].keys():
            method_name = method_names[m]["name"]
            try:
                df = load_app_dse(logdir, app, m, cols=columns)
                print(df.info())
            except FileNotFoundError:
                df = pd.DataFrame()
            sd_count = df["storage distribution size"].count() if "throughput" in df else "-"
            max_th = df["throughput"].max()
            duration = df["cumulative duration"].max() if "cumulative duration" in df else "-"
            print(app, m, max_th, app_max_throughput)
            finished = math.isclose(float(max_th), float(app_max_throughput), rel_tol=1e-5)
            pareto = extract_pareto(df[["throughput", "storage distribution size"]])
            pareto_count = pareto["storage distribution size"].count() if finished else "-"
            list_of_dict += [{"graph": app_name,
                              "#task": app_task_count,
                              "method": method_name,
                              "#SD": sd_count,
                              "#Pareto": pareto_count,
                              "Duration": int(duration),
                              "Finished": finished}]

    return pd.DataFrame(list_of_dict)


def compare_methods(df, methods):


    # Ensure the DataFrame is in the expected format
    df = df.set_index(["graph", "#task", "method"])

    # Initialize an empty DataFrame for comparison
    comparison_df = pd.DataFrame()

    # Loop through each method, extract its data, and add it to the comparison DataFrame
    for method in methods:
        method_data = df.xs(method, level='method')
        # If the comparison DataFrame is empty, just add the first method's data
        if comparison_df.empty:
            comparison_df = method_data.rename(columns={'Duration': f'Duration_{method}'})
        else:
            # If not, join with the existing data
            comparison_df = comparison_df.join(method_data['Duration'].rename(f'Duration_{method}'))

    # Plotting
    plt.figure(figsize=(10, 6))
    colors = ['skyblue', 'salmon', 'lightgreen', 'orange', 'purple', 'yellow']  # Extend this list for more methods
    position = 0  # Initial bar position

    for idx, method in enumerate(methods):
        comparison_df[f'Duration_{method}'].plot(kind='bar', color=colors[idx % len(colors)],
                                                 position=len(methods) - position, width=1 / (1 + len(methods)),
                                                 label=method)
        position += 1  # Move position for the next bar

    plt.ylabel('Duration')
    plt.title('Comparison of Duration between Methods')
    plt.xticks(rotation=45)
    plt.legend()
    plt.tight_layout()
    plt.show()

def compare_methods_with_baseline(df, methods, baseline):

    # Ensure the DataFrame is set with the right multi-index
    df2 = df.set_index(["graph", "#task", "method"])

    # Initialize a dictionary to hold data for all methods
    normalized_data = {}

    # Get baseline data for normalization
    baseline_data = df2.xs(baseline, level='method')

    # Iterate through methods to calculate normalized duration
    for method in methods:
        method_data = df2.xs(method, level='method')
        # Calculate normalized duration as current method duration divided by baseline method duration
        # Join on index to align corresponding tasks, fill missing values to handle tasks not present in both methods
        normalized_duration = method_data['Duration'].div(baseline_data['Duration'], fill_value=1)
        # Store normalized data
        normalized_data[method] = normalized_duration

    # Convert the dictionary to a DataFrame for plotting
    comparison_df = pd.DataFrame(normalized_data)

    # Plotting
    plt.figure(figsize=(10, 6))
    comparison_df.plot(kind='bar', width=0.8)
    plt.ylabel(f'Duration (Normalized by {baseline})')
    plt.title(f'Comparison of Duration between Methods Normalized by {baseline}')
    plt.xticks(rotation=45)
    plt.legend()
    plt.tight_layout()
    plt.show()


def sanity_check(log_dir, applications, methods):
    for app_key, app_values in applications.items():

        if not "max_throughput" in app_values or not "task_count" in app_values:
            continue

        for method_key, method_values in methods.items():

            app_name = app_values["name"]
            app_task_count = app_values["task_count"]
            method_name = method_values["name"]

            try:
                df = load_app_dse(log_dir, app_key, method_key, cols=["throughput",
                                                                               "storage distribution size",
                                                                               "cumulative duration",
                                                                               "feedback quantities"])
            except FileNotFoundError:
                continue
            except ValueError:
                continue

            # assert it finished
            max_th = df["throughput"].max()
            app_max_throughput = app_values["max_throughput"]
            finished = math.isclose(max_th, app_max_throughput, rel_tol=1e-5)
            assert (finished)

            # assert there is no duplicates
            duplicates_count = len(df["feedback quantities"]) - len(df["feedback quantities"].drop_duplicates())
            assert (duplicates_count == 0)

            print(app_key, method_key, finished, duplicates_count)

    return True

def load_app_dse(
        log_dir,
        appname,
        method,
        cols=None,
):
    if cols is None:
        cols = ["throughput", "cumulative duration", "storage distribution size"]
    filename = f"{log_dir}/{appname}_{method}.txt"
    if cols:
        df = pd.read_csv(filename, usecols=cols, na_values="-")
    else:
        df = pd.read_csv(filename)
    return df


def extract_pareto(df):
    # sort by sd
    pareto = df[df["throughput"] > 0].sort_values("storage distribution size").copy()

    # update th to max possible (assume ordered by sd)
    pareto["throughput"] = pareto["throughput"].cummax()

    # Take the max th for each sd
    pareto = pareto.groupby("storage distribution size").max().reset_index()

    # Take the min sd for each th
    pareto = pareto.groupby("throughput").min().reset_index()

    return pareto


def plot_dse(df, dsename=None, dsecolor=None):
    if len(df) == 0:
        return (0, 0)

    x = df["cumulative duration"]
    y = df["throughput"]

    if len(y) == 0:
        return (0, 0)

    steplines = plt.step(
        x, y, where="post", color=dsecolor, linewidth=0.4, alpha=0.5, label=dsename
    )  # 'C0o'
    current_color = steplines[0].get_color()

    df["cummaxth"] = df["throughput"].cummax()
    dfgb = df.groupby("cummaxth")

    x2 = dfgb.min()["cumulative duration"]
    y2 = dfgb.max()["throughput"]

    pareto = extract_pareto(df)
    x2, y2 = pareto["cumulative duration"], pareto["throughput"]

    markerline, stemlines, baseline = plt.stem(
        x2, y2, current_color, use_line_collection=True, basefmt=" "
    )
    plt.setp(stemlines, "linewidth", 0.5)
    plt.setp(markerline, "color", current_color)
    plt.setp(markerline, "markersize", 1)

    return (x2.max(), y2.max())


def plot_pareto(df, dsename=None, dsecolor=None):
    if len(df) == 0:
        return (0, 0)

    pareto = extract_pareto(df)

    # Add period
    pareto["period"] = 1 / pareto["throughput"]
    df["period"] = 1 / df["throughput"]

    x, y = pareto["storage distribution size"], pareto["period"]
    if len(y) == 0:
        return (0, 0)

    markersize = 3
    linewidth = 1

    scatterpoints = plt.scatter(
        df["storage distribution size"],
        df["period"],
        s=markersize,
        alpha=0.1,
        label=None,
    )
    steplines = plt.step(
        x, y, where="post", color=dsecolor, linewidth=linewidth, alpha=1, label=dsename
    )  # 'C0o'
    current_color = steplines[0].get_color()
    steppoints = plt.plot(x, y, "C2o", markersize=markersize, alpha=0.5, label=None)
    plt.setp(steppoints, "color", current_color)
    plt.setp(scatterpoints, "color", current_color)

    return (x.max(), y.max())


def plot_app_dse(logdir, appname, methods):
    total_time = time.time()
    ymax = 0
    xmax = 0
    for method, color in methods.items():
        start_time = time.time()
        print("Load", appname, method)
        df = load_app_dse(logdir, appname, method)
        print("Loaded after", time.time() - start_time, "sec.")

        start_time = time.time()
        print("Plot", appname, method)

        cxmax, cymax = plot_dse(df, methods[method], color)
        xmax = max(xmax, cxmax)
        ymax = max(ymax, cymax)

        print("Plotted after", time.time() - start_time, "sec.")

    plt.xlabel("Execution time (ms)")
    plt.ylabel("Throughput (Hz)")
    plt.title(f"{appname}")
    plt.legend(loc="upper left", ncol=2, borderaxespad=0.5)

    if ymax:
        plt.ylim(bottom=0, top=1.3 * ymax)

    if xmax:
        plt.xlim(left=0, right=1.3 * xmax)

    print("Finished in", time.time() - total_time, "sec.")
    print("")


def plot_app_pareto(logdir, appname, methods):
    total_time = time.time()
    ymax = 0
    xmax = 0

    for method_key in methods.keys():
        method_name = methods[method_key]["name"]
        color = methods[method_key]["color"]
        start_time = time.time()
        print("Load", appname, method_key, method_name)
        try:
            df = load_app_dse(
                logdir, appname, method_key, cols=["throughput", "storage distribution size"]
            )
            print("Loaded after", time.time() - start_time, "sec.")
            start_time = time.time()
            print("Plot", appname, method_name)
            cxmax, cymax = plot_pareto(df, method_name, color)
            xmax = max(xmax, cxmax)
            ymax = max(ymax, cymax)
            print("Plotted after", time.time() - start_time, "sec.")
        except:
            print("Loaded FAILED after", time.time() - start_time, "sec.")

    plt.xlabel("Storage Distribution")
    plt.ylabel("Period (sec)")
    plt.title(f"{appname}")
    plt.legend(loc="upper left", ncol=2, borderaxespad=0.5)

    if ymax:
        plt.ylim(bottom=0, top=1.3 * ymax)

    if xmax:
        plt.xlim(left=0, right=1.3 * xmax)

    print("Finished in", time.time() - total_time, "sec.")
    print("")


def plot_all(logdir, graphs, methods, plotfunc=plot_app_dse, outputname=None):
    total = len(graphs)
    subx = max(1, int(math.sqrt(total)))
    suby = int(total / subx)
    assert subx >= 1
    while subx * suby < total:
        suby += 1

    print(f"Produce subplot of {subx} x {suby}")
    fig = plt.figure(figsize=(suby * 5, subx * 5))

    fig.subplots_adjust(hspace=0.4, wspace=0.4)

    for i, name in zip(range(1, len(graphs) + 1), graphs):
        _ = fig.add_subplot(subx, suby, i)
        plotfunc(logdir, name, methods)

    print("Tight the layout...")
    fig.tight_layout()

    if outputname:
        print("Save the file:", outputname)
        plt.savefig(outputname)
        plt.clf()
        plt.cla()  # Clear axis
    else:
        print("Show the plot")
        plt.show()


def gen_minsize(logdir, graphs, methods, outputname="/dev/stdout"):
    res = {"name": []}
    for m in methods:
        res[m] = []
    for i, name in zip(range(1, len(graphs) + 1), graphs):
        res["name"].append(name)
        for m in methods:
            df = load_app_dse(logdir, name, m, cols=["throughput", "storage distribution size"])
            v = df[df["throughput"] > 0]["storage distribution size"].min() if "throughput" in df else "-"
            res[m].append(v)

    df = pd.DataFrame(res)[["name"] + [*methods]]
    df = df.rename(columns={
        "name": "Graph"
    })
    colformat = "|".join([""] + ["l"] * df.index.nlevels + ["r"] * df.shape[1] + [""])

    latex = df.to_latex(
        float_format="{:0.1f}".format, column_format=colformat, index=False
    )
    latex = latex.replace("NAN", "-")
    fd = open(outputname, 'w')
    fd.write(latex)
    fd.close()


def gen_dsetable(logdir, graphs, methods, outputname="/dev/stdout"):
    res = {"name": []}
    for m in methods:
        res[methods[m]["name"]] = []
    for i, name in zip(range(1, len(graphs) + 1), graphs):
        res["name"].append(name)
        for m in methods:
            df = load_app_dse(logdir, name, m, cols=["throughput", "storage distribution size"])
            v = df["storage distribution size"].count() if "throughput" in df else "-"
            res[methods[m]["name"]].append(v)

    df = pd.DataFrame(res)[["name"] + [methods[m]["name"] for m in methods]]
    df = df.rename(columns={
        "name": "Graph"
    })
    colformat = "|".join([""] + ["l"] * df.index.nlevels + ["r"] * df.shape[1] + [""])

    latex = df.to_latex(
        float_format="{:0.1f}".format, column_format=colformat, index=False
    )
    latex = latex.replace("NAN", "-")
    fd = open(outputname, 'w')
    fd.write(latex)
    fd.close()
    return df


def plot_all_pareto(log_dir, graphs, methods, output_name=None):
    plot_all(log_dir, graphs, methods, plotfunc=plot_app_pareto, outputname=output_name)


def plot_all_dse(log_dir, graphs, methods, output_name=None):
    plot_all(log_dir, graphs, methods, plotfunc=plot_app_dse, outputname=output_name)


if __name__ == "__main__":
    import argparse
    import glob

    #methods = {"KDSE": "red", "DKDSE": "purple", "ADKDSE": "black", "PDSE": "green"}
    #method_name = {"KDSE": "KDSE", "DKDSE": "K2DSE", "PDSE": "PDSE", "ADKDSE": "Approx K2DSE"}

    parser = argparse.ArgumentParser(description="Generate DSE Plots")
    parser.add_argument(
        "graphs", metavar="G", type=str, nargs="*", help="Names of graphs to process"
    )
    parser.add_argument(
        "--logdir", type=str, help="location of log dirs", required=True
    )
    parser.add_argument(
        "--opareto",
        type=str,
        help="location of the output dse plot file",
        required=False,
    )
    parser.add_argument(
        "--odse",
        type=str,
        help="location of the output pareto plot file",
        required=False,
    )

    parser.add_argument(
        "--dsetable",
        type=str,
        help="location of the output minimal buffer size table file",
        required=False,
    )
    args = parser.parse_args()

    logdir = args.logdir
    graphs = args.graphs

    if len(graphs) == 0:
        graphs = list(
            set(
                [x.split("/")[-1].split("_dselog")[0] for x in glob.glob(logdir + "/*")]
            )
        )
    print("Process graphs:", graphs)
    if len(graphs) == 0:
        raise SystemExit(0)

    process = psutil.Process(os.getpid())
    start_mem = process.memory_info().rss
    assert(sanity_check(logdir, applications=application_names, methods=method_names))

    if args.odse:
        print("Generate DSE output")
        plot_all_dse(log_dir=logdir, graphs=graphs, methods=method_names, output_name=args.odse)

    if args.opareto:
        print("Generate pareto output")
        plot_all_pareto(log_dir=logdir, graphs=graphs, methods=method_names, output_name=args.opareto)

    if args.dsetable:
        print("Generate minimal size table")
        gen_dsetable(logdir=logdir, graphs=graphs, methods=method_names, outputname=args.dsetable)


    end_mem = process.memory_info().rss
    print(
        "Memory usage from",
        int(start_mem / (2 ** 20)),
        " MB to",
        int(end_mem / (2 ** 20)),
        "MB",
    )
