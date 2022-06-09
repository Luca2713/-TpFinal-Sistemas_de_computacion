import env_var_functions as env
import PySimpleGUI as sg
import numpy as np
import requests
import json
import time

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from random import randint

url = f'{env.get_base_url()}{env.get_endpoint()}'
port = env.get_raspi_p1()
time_s = 0
x_vals = np.zeros(10)
y_vals = np.zeros(10)


def get_sensor_data():
    global time_s
    global x_vals
    global y_vals
    global port

    x_vals = np.roll(x_vals, -1)
    x_vals[9] = time_s
    time_s += 1

    y_vals = np.roll(y_vals, -1)

    req = {'in': port}
    x = requests.get(url, json=req)
    res = json.loads(x.text)

    if(res["code"] == 200):
        print("ok")
        print(y_vals)
        y_vals[9] = res["out"]

    else:
        print("Error")


def draw_figure(canvas, figure, loc=(0, 0)):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg


def main():
    global port
    global time_s
    global x_vals
    global y_vals

    # define the form layout
    layout = [[sg.Text('Animated Matplotlib', size=(40, 1),
                       justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 480), key='-CANVAS-')],
              [sg.Button('Toggle', size=(10, 1), pad=((280, 0), 3), font='Helvetica 14')]]

    # create the form and show it without the plot
    window = sg.Window('Demo Application - Embedding Matplotlib In PySimpleGUI',
                       layout, finalize=True)

    canvas_elem = window['-CANVAS-']
    canvas = canvas_elem.TKCanvas

    # draw the initial plot in the window
    fig = Figure()
    ax = fig.add_subplot(111)
    ax.set_xlabel("X axis")
    ax.set_ylabel("Y axis")

    ax.set_xlim(time_s - 10, time_s)
    ax.grid()

    fig_agg = draw_figure(canvas, fig)

    while(True):

        event, values = window.read(timeout=10)

        if(event == "Toggle"):
            time_s = 0
            x_vals = np.zeros(10)
            y_vals = np.zeros(10)
            
            if(port == env.get_raspi_p2()):
                port = env.get_raspi_p1()
            else:
                port = env.get_raspi_p2()

        if(event == sg.WIN_CLOSED):
            exit()

        get_sensor_data()

        ax.cla()                    # clear the subplot
        ax.grid()                   # draw the grid
        ax.plot(x_vals, y_vals,  color='purple')
        fig_agg.draw()
        time.sleep(1)


if __name__ == '__main__':
    main()
