#!/usr/bin/env python3

import sys
import re
import argparse
import copy
import matplotlib.pyplot as plt
from matplotlib import rcParams, rc
from matplotlib.ticker import FormatStrFormatter

rcParams['font.family'] = 'sans-serif'
rcParams['font.sans-serif'] = ['Arial']
rcParams['lines.linewidth'] = 1.5
font = {'size' : 10}
rc('font', **font)
figsize = (4,3)
rcParams["legend.framealpha"] = 1.0
rcParams["legend.frameon"] = False
line_style_list = ["k-", "b--", "r:", "g-.", "b--x", "r:x", "g-.x", "b--o", "r:o", "g-.o",]

#floating_point_regex = r"[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?"

def read_console_output_file(input_file_path):
    with open(input_file_path, 'r') as input_file:
        console_output = input_file.read()
    return console_output

def get_repeated_patterns(input_string, regex_string):
    input_string = input_string[:]
    results = re.finditer(regex_string, input_string, flags=(re.DOTALL))
    each_time_step_start_indices = []
    for ith_result in results:
        each_time_step_start_indices.append(ith_result.start())
    each_time_step_output = []
    for ith_result_start in each_time_step_start_indices[::-1]: # Reverse order
        each_time_step_output.append(input_string[ith_result_start:])
        input_string = input_string[:ith_result_start]
    each_time_step_output = each_time_step_output[::-1]
    return each_time_step_output

def get_time_step_and_time(input_string):
    regex_string = r"Time Step (\d+), time = (\d+).*?"
    results = re.findall(regex_string, input_string)
    return int(results[0][0]), float(results[0][1])

def get_step_and_res(input_string, regex_string):
    results = re.findall(regex_string, input_string)
    return int(results[0][0]), float(results[0][1])

def get_individual_var_res(input_string):
    regex_string = r"(\w_+):\s*([\d.e+-]*)"
    regex_string = r"^\s*(\w+):\s*([\d\.e\+-]+)"
    results = re.finditer(regex_string, input_string, flags=(re.DOTALL|re.MULTILINE))
    individual_var_res = {}
    for ith_result in results:
        individual_var_res[ith_result.group(1)] = float(ith_result.group(2))
    return individual_var_res

def plot_time_step_data(args, time_step_data_list):
    for i_time_step, ith_time_step_data in enumerate(time_step_data_list):
        if args.time_step:
            time_step = ith_time_step_data["time_step"]
            if time_step != args.time_step:
                continue
        plot_one_data(args, ith_time_step_data)
           
def plot_one_data(args, one_data):
    # nl plot
    fig, ax = plt.subplots(figsize=figsize)
    nl_step_list = one_data["nl_step_list"]
    nl_res_list = one_data["nl_res_list"]
    line_style_list = ["k-", "b--", "r:", "g-.", "b--x", "r:x", "g-.x", "b--o", "r:o", "g-.o",]
    line_counter = 0
    if args.nl_semilogy:
        plt.semilogy(nl_step_list, nl_res_list, line_style_list[line_counter], label="Total")
    else:
        plt.plot(nl_step_list, nl_res_list, "b-")
    line_counter += 1
    if args.individual_var:
        print(one_data.keys())
        var_name_list = list(one_data["nl_data"][0]["individual_var_res"].keys())
        var_name_list.sort()
        for ith_var_name in var_name_list:
            var_res_list = []
            for jth_nl_data in one_data["nl_data"]:
                var_res_list.append(jth_nl_data["individual_var_res"][ith_var_name])
            plt.semilogy(nl_step_list, var_res_list, line_style_list[line_counter], label=ith_var_name)
            line_counter += 1
    plt.xlabel("Non-linear step")
    plt.ylabel("$|L|_2$ residual")
    legend = ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.25, 0.001, 0.01), ncol=2)
    plt.tight_layout()
    plt.savefig(args.nl_plot_path, bbox_extra_artists=(legend,), bbox_inches='tight')
    
    # linear plot
    fig, ax = plt.subplots(figsize=figsize)
    line_style_list = ["k--", "b--", "r--", "g--", "k:", "b:", "r:", "g:", "r:o", "g-.o",]
    line_counter = 0
    for i_lin_data, ith_lin_data in enumerate(one_data["nl_data"]):
        lin_step_list = ith_lin_data["lin_step_list"]
        if len(lin_step_list) == 0:
            continue
        lin_res_list = ith_lin_data["lin_res_list"]
        if args.lin_semilogy:
            plt.semilogy(lin_step_list, lin_res_list, line_style_list[line_counter], label=i_lin_data)
        elif args.lin_loglog:
            plt.loglog(lin_step_list, lin_res_list, line_style_list[line_counter], label=i_lin_data)
        else:
            plt.plot(lin_step_list, lin_res_list, line_style_list[line_counter], label=i_lin_data)
        line_counter += 1
    plt.xlabel("Linear step")
    plt.ylabel("$|L|_2$ residual")
    legend = ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.25, 0.001, 0.01), ncol=3)
    plt.tight_layout()
    plt.savefig(args.lin_plot_path, bbox_extra_artists=(legend,), bbox_inches='tight')


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
                nl_step, nl_res = get_step_and_res(jth_nl_output, r"(\d+)\s*Nonlinear\s*\|R\|\s*=\s*([\d.e+-]*)")
                nl_step_list.append(nl_step)
                nl_res_list.append(nl_res)
                each_lin_output_list = get_repeated_patterns(jth_nl_output, r"\d+\s*Linear\s*\|R\|\s*=\s*[\d.e+-]*")

                print("    Number of linear iterations : {:d}".format(len(each_lin_output_list)))
                lin_step_list = []
                lin_res_list = []
                for kth_lin_output in each_lin_output_list:
                    lin_step, lin_res = get_step_and_res(kth_lin_output, r"(\d+)\s*Linear\s*\|R\|\s*=\s*([\d.e+-]*)")
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
        print(len(time_step_data_list))
        plot_time_step_data(args, time_step_data_list)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Plots non-linear and linear residual.')
    parser.add_argument('--in', '-i', dest='input_file_path', type=str)
    parser.add_argument('--out', '-o', dest='output_file_path', type=str)
    parser.add_argument('--transient', '-T', dest='is_transient', action='store_true', default=True)
    parser.add_argument('--steady', '-S', dest='is_steady', action='store_true', default=True)
    parser.add_argument('--individual_var', '-I', dest='individual_var', action='store_true', default=True)
    #parser.add_argument('--vars', '-e', dest='individual_var', action='store_true', default=True)

    parser.add_argument('--nl_semilogy', dest='nl_semilogy', action='store_true', default=False)
    parser.add_argument('--nl_plot_path', dest='nl_plot_path', type=str)
    parser.add_argument('--lin_semilogy', dest='lin_semilogy', action='store_true', default=False)
    parser.add_argument('--lin_loglog', dest='lin_loglog', action='store_true', default=False)
    parser.add_argument('--lin_plot_path', dest='lin_plot_path', type=str)
    parser.add_argument('--time_step', dest='time_step', type=int)
    parser.add_argument('--nl_step', dest='nl_step', type=int)
    args = parser.parse_args()

    analyze_console_output(args)
    
