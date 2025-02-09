import matplotlib.pyplot as plt
import numpy as np
import os
import math


def moving_average(a, n=3):
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1 :] / n


def firstPlotRoutine():
    dirname = os.path.dirname(__file__)
    sampledData = np.loadtxt(
        os.path.join(dirname, "./../sampled-data/sampled-data-1.txt"), unpack=True
    )
    print("sampled data size: ", sampledData.size)
    dataLength = sampledData.size / 100

    delay = 175 / 10
    tau = dataLength / 5

    xAxis = np.linspace(0, dataLength, sampledData.size)
    xAverage = np.linspace(0, dataLength, sampledData.size - 4)

    xApprox = np.linspace(0, dataLength, sampledData.size)
    d = np.heaviside(xApprox - delay, 1)
    offset = 111
    yApprox = d * ((350 - offset) * (1.0 - np.exp(-((xApprox - delay) / tau)))) + offset

    fig, axs = plt.subplots(3)

    axs[0].plot(xAverage, moving_average(sampledData, 5))
    axs[1].plot(xAxis, sampledData, xAxis, yApprox)
    axs[2].plot(xAxis, yApprox)

    axs[0].grid()
    axs[1].grid()
    axs[2].grid()

    axs[0].set(
        xlabel="time (seconds)",
        ylabel="averag ADC Value",
        title="Graph Estimation",
    )
    axs[1].set(
        ylabel="ADC Value and Graph Estimation",
    )
    axs[1].legend(["Raw Data", "Estimation Data"])

    axs[2].set(
        ylabel="Estimation Graph",
    )

    # fig.savefig("test.png")
    plt.show()


def main():
    fileName = [
        "./../../../sampledData/dataa2.csv",
        "./../../../sampledData/11-03-1048_4096-over-2_2MHZ",
        "./../../../sampledData/11-03-1048_4096-over-4_2MHZ",
        "./../../../sampledData/11-03-1048_4096-over-6_2MHZ",
        "./../../../sampledData/11-03-1048_4096-over-16_2MHZ",
        "./../../../sampledData/11-03-1048_4096-over-20_2MHZ",
    ]

    fig, ax = plt.subplots(len(fileName))
    for index, file in enumerate(fileName):
        dirname = os.path.dirname(__file__)
        sampledData = np.loadtxt(
            os.path.join(dirname, file),
            unpack=True,
        )
        print("sampled data size: ", sampledData.size)
        dataLength = sampledData.size / 100

        yAxis = moving_average(sampledData, 500)
        xAxis = np.linspace(0, dataLength, yAxis.size)

        ax[index].plot(xAxis, yAxis)
        ax[index].grid()
        ax[index].legend(
            title=file.split("/")[-1],
        )

    plt.show()
