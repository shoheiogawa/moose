#!/usr/bin/env python3

import re
import argparse
import copy
import matplotlib.pyplot as plt
from matplotlib import rcParams, rc
from matplotlib.ticker import FormatStrFormatter
from matplotlib.ticker import MaxNLocator

rcParams['font.family'] = 'sans-serif'
rcParams['font.sans-serif'] = ['Arial']
rcParams['lines.linewidth'] = 1.5
font = {'size' : 10}
rc('font', **font)
figsize = (4,3)
rcParams["legend.framealpha"] = 1.0
rcParams["legend.frameon"] = False

# nl plot settings
line_style_list_nl_plot = ["k-", "b--", "r:", "g-.", "k-x", "b--x", "r:x", "g-.x", "k-o", "b--o", "r:o", "g-.o"]

# nl plot legend
ncol_nl_plot = 2
legend_loc_nl_plot = "upper center"
bbox_to_anchor_nl_plot = (0.5, 1.25, 0.001, 0.01)

# nl plot axis
xlabel_nl_plot = "Non-linear step"
ylabel_nl_plot = "$|L|_2$ residual"

# lin plot settings
line_style_list_lin_plot = ["b--", "b-.", "b:", "b-", "r--", "r-.", "r:", "r-:", "k-.", "b-.", "r-.", "g-.",]

# lin plot axis
ncol_lin_plot = 3
legend_loc_lin_plot = legend_loc_nl_plot
bbox_to_anchor_lin_plot = (0.5, 1.25, 0.001, 0.01)

# lin plot axis
xlabel_lin_plot = "Linear step"
ylabel_lin_plot = "$|L|_2$ residual"

def read_console_output_file(input_file_path):
    with open(input_file_path, 'r') as input_file:
        console_output = input_file.read()
    return console_output

def get_repeated_patterns(input_string, regex_string):
    input_string = input_string[:]
    results = re.finditer(regex_string, input_string, flags=(re.DOTALL))
    pattern_start_indices = []
    for ith_result in results:
        pattern_start_indices.append(ith_result.start())
    pattern_output = []
    for ith_result_start in pattern_start_indices[::-1]: # Reverse order
        pattern_output.append(input_string[ith_result_start:])
        input_string = input_string[:ith_result_start]
    pattern_output = pattern_output[::-1] # Reverse order
    return pattern_output

def get_time_step_and_time(input_string):
    regex_string = r"Time Step (\d+), time = ([\d.]+).*?"
    results = re.findall(regex_string, input_string)
    return int(results[0][0]), float(results[0][1])

def get_step_and_res(input_string, regex_string):
    results = re.findall(regex_string, input_string)
    return int(results[0][0]), float(results[0][1])

def get_nl_step_and_res(input_string):
    regex_string = r"(\d+)\s*Nonlinear\s*\|R\|\s*=\s*([\d.e+-]*)"
    results = re.findall(regex_string, input_string)
    return int(results[0][0]), float(results[0][1])

def get_lin_step_and_res(input_string):
    regex_string = r"(\d+)\s*Linear\s*\|R\|\s*=\s*([\d.e+-]*)"
    results = re.findall(regex_string, input_string)
    return int(results[0][0]), float(results[0][1])

def get_individual_var_res(input_string):
    regex_string = r"(\w_+):\s*([\d.e+-]*)"
    regex_string = r"^\s*(\w+):\s*([\d\.e\+-]+)"
    results = re.finditer(regex_string, input_string, flags=(re.MULTILINE))
    individual_var_res = {}
    for ith_result in results:
        individual_var_res[ith_result.group(1)] = float(ith_result.group(2))
    return individual_var_res

def plot_time_step_data(args, time_step_data_list):
    for i_time_step, ith_time_step_data in enumerate(time_step_data_list):
        if args.time_step:
            time_step = ith_time_step_data["time_step"]
            if not time_step in args.time_step:
                continue
        plot_one_data(args, ith_time_step_data)

def get_plot_func_nl(args):
    if args.nl_plot_type:
        nl_plot_type = args.nl_plot_type
        if nl_plot_type == "loglog":
            plot_func = plt.loglog
        elif nl_plot_type == "semilogx":
            plot_func = plt.semilogx
        elif nl_plot_type == "semilogy":
            plot_func = plt.semilogy
        elif nl_plot_type == "plot":
            plot_func = plt.plot
        else:
            ValueError("Argument for --nl_plot_path is not valid.")
    else:
        plot_func = plt.plot
    return plot_func

def get_plot_func_lin(args):
    if args.lin_plot_type:
        lin_plot_type = args.lin_plot_type
        if lin_plot_type == "loglog":
            plot_func = plt.loglog
        elif lin_plot_type == "semilogx":
            plot_func = plt.semilogx
        elif lin_plot_type == "semilogy":
            plot_func = plt.semilogy
        elif lin_plot_type == "plot":
            plot_func = plt.plot
        else:
            ValueError("Argument for --lin_plot_path is not valid.")
    else:
        plot_func = plt.plot
    return plot_func

def plot_one_data_nl(args, one_data):
    fig, ax = plt.subplots(figsize=figsize)
    nl_step_list = one_data["nl_step_list"]
    nl_res_list = one_data["nl_res_list"]
    line_counter = 0
    plot_func_nl = get_plot_func_nl(args)
    plot_func_nl(nl_step_list, nl_res_list, line_style_list_nl_plot[line_counter], label="Total")
    line_counter += 1
    if args.individual_var:
        var_name_list = list(one_data["nl_data"][0]["individual_var_res"].keys())
        var_name_list.sort()
        for ith_var_name in var_name_list:
            if args.vars:
                if not ith_var_name in args.vars:
                    continue
            var_res_list = []
            for jth_nl_data in one_data["nl_data"]:
                var_res_list.append(jth_nl_data["individual_var_res"][ith_var_name])
            plot_func_nl(nl_step_list, var_res_list, line_style_list_nl_plot[line_counter], label=ith_var_name)
            line_counter += 1
    plt.xlabel(xlabel_nl_plot)
    plt.ylabel(ylabel_nl_plot)
    if args.nl_iter_range:
        plt.xlim(args.nl_iter_range)
    if args.nl_res_range:
        plt.ylim(args.nl_res_range)
    legend = ax.legend(loc=legend_loc_nl_plot, bbox_to_anchor=bbox_to_anchor_nl_plot, ncol=ncol_nl_plot)
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    plt.tight_layout()
    print("Saving nonlinear residual plot")
    plt.savefig(args.nl_plot_path, bbox_extra_artists=(legend,), bbox_inches='tight')

def plot_one_data_lin(args, one_data):
    fig, ax = plt.subplots(figsize=figsize)
    plot_func_lin = get_plot_func_lin(args)
    line_counter = 0
    for i_lin_data, ith_lin_data in enumerate(one_data["nl_data"]):
        lin_step_list = ith_lin_data["lin_step_list"]
        if len(lin_step_list) == 0:
            continue
        lin_res_list = ith_lin_data["lin_res_list"]
        plot_func_lin(lin_step_list, lin_res_list, line_style_list_lin_plot[line_counter], label=i_lin_data)
        line_counter += 1
    plt.xlabel(xlabel_lin_plot)
    plt.ylabel(ylabel_lin_plot)
    if args.lin_iter_range:
        plt.xlim(args.lin_iter_range)
    if args.lin_res_range:
        plt.ylim(args.lin_res_range)
    legend = ax.legend(loc=legend_loc_lin_plot, bbox_to_anchor=bbox_to_anchor_lin_plot, ncol=ncol_lin_plot)
    if args.lin_plot_type == "plot" or args.lin_plot_type == "semilogy":
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    plt.tight_layout()
    print("Saving linear residual plot")
    plt.savefig(args.lin_plot_path, bbox_extra_artists=(legend,), bbox_inches='tight')

def plot_one_data(args, one_data):
    # Nonlinear plot
    plot_one_data_nl(args, one_data)
    # Linear plot
    plot_one_data_lin(args, one_data)

def analyze_console_output(args):
    print("Analyzing console output...")
    console_output = read_console_output_file(args.input_file_path)
    if args.is_transient:
        each_time_step_output = get_repeated_patterns(console_output, r"Time Step \d+, time = \d+.*?")
        each_time_step_output = each_time_step_output[1:]
        time_step_data_list = []
        for ith_output in each_time_step_output:
            ith_time_step_data = {}
            time_step, time = get_time_step_and_time(ith_output)
            print("Time step : {:d}".format(time_step))
            each_nl_output_list = get_repeated_patterns(ith_output, r"\d+\s*Nonlinear\s*\|R\|\s*=\s*[\d.e+-]*")
            print("  Number of nonlinear iterations : {:d}".format(len(each_nl_output_list)))
            nl_step_list = []
            nl_res_list = []
            nl_data = []
            for jth_nl_output in each_nl_output_list:
                jth_nl_data = {}
                if args.individual_var:
                    result = re.search(r"\d+\s*Nonlinear\s*\|R\|\s*=\s*[\d.e+-]*", ith_output)
                    if result:
                        individual_var_output = ith_output[:result.end()]
                        ith_output = ith_output[result.end():]
                        individual_var_res = get_individual_var_res(individual_var_output)
                        print("    Individual var res :", individual_var_res)
                    jth_nl_data["individual_var_res"] = individual_var_res
                nl_step, nl_res = get_nl_step_and_res(jth_nl_output)
                nl_step_list.append(nl_step)
                nl_res_list.append(nl_res)
                each_lin_output_list = get_repeated_patterns(jth_nl_output, r"\d+\s*Linear\s*\|R\|\s*=\s*[\d.e+-]*")

                print("    Number of linear iterations : {:d}".format(len(each_lin_output_list)))
                lin_step_list = []
                lin_res_list = []
                for kth_lin_output in each_lin_output_list:
                    lin_step, lin_res = get_lin_step_and_res(kth_lin_output)
                    lin_step_list.append(lin_step)
                    lin_res_list.append(lin_res)
                jth_nl_data["lin_step_list"] = lin_step_list
                jth_nl_data["lin_res_list"] = lin_res_list
                jth_nl_data["nl_step"] = nl_step
                jth_nl_data["nl_res"] = nl_res
                nl_data.append(jth_nl_data)
            ith_time_step_data["nl_data"] = nl_data
            ith_time_step_data["time_step"] = time_step
            ith_time_step_data["time"] = time
            ith_time_step_data["nl_res_list"] = nl_res_list
            ith_time_step_data["nl_step_list"] = nl_step_list
            time_step_data_list.append(ith_time_step_data)
        print("Output data stored.")
        plot_time_step_data(args, time_step_data_list)

if __name__ == "__main__":
    # Command line argument definitions
    parser = argparse.ArgumentParser(description='Plots non-linear and linear residual.')
    parser.add_argument('--in', '-i', dest='input_file_path', type=str)
    parser.add_argument('--out', '-o', dest='output_file_path', type=str)
    parser.add_argument('--transient', '-T', dest='is_transient', action='store_true')
    parser.add_argument('--time_step', '-t', dest='time_step', type=int, nargs='*')
    parser.add_argument('--steady', '-S', dest='is_steady', action='store_true')
    parser.add_argument('--individual_var', '-I', dest='individual_var', action='store_true')
    parser.add_argument('--vars', '-v', dest='vars', nargs='*')

    parser.add_argument('--nl_plot_path', dest='nl_plot_path', type=str)
    parser.add_argument('--nl_plot_type', dest='nl_plot_type', type=str)
    parser.add_argument('--nl_res_range', dest='nl_res_range', type=float, nargs=2)
    parser.add_argument('--nl_iter_range', dest='nl_iter_range', type=int, nargs=2)
    parser.add_argument('--lin_plot_path', dest='lin_plot_path', type=str)
    parser.add_argument('--lin_plot_type', dest='lin_plot_type', type=str)
    parser.add_argument('--lin_res_range', dest='lin_res_range', type=float, nargs=2)
    parser.add_argument('--lin_iter_range', dest='lin_iter_range', type=float, nargs=2)

    args = parser.parse_args()

    # Checking if valid argument values are given or not. If invalid, ValueError is raised.
    get_plot_func_nl(args)
    get_plot_func_lin(args)
    
    analyze_console_output(args)

