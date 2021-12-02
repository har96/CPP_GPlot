#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>


class GNUPlot {
    public:

        GNUPlot() : title("Untitled"), xlabel("x"), ylabel("y"), data_ind(0) {};

        template<class ContainerX, class ContainerY>
        void plot(ContainerX const& x, ContainerY const& y,
                  std::string name="",
                  bool points=false,
                  std::string color="",
                  int type=0,
                  int weight=2);

        template<class ContainerX, class ContainerY>
        void scatter(ContainerX const& x, ContainerY const& y,
                std::string name="",
                std::string style="");

        inline void setTitle(std::string title) { this->title = title; };
        inline void setXLabel(std::string xlabel) { this->xlabel = xlabel; };
        inline void setYLabel(std::string ylabel) { this->ylabel = ylabel; };

        void show();

    private:
        std::string title;
        std::string xlabel, ylabel;

        std::stringstream plotting_data;
        std::stringstream plotting_cmds;

        FILE * plot_process;

        int data_ind;

        template<class ContainerX, class ContainerY>
        void add_points(ContainerX const& x, ContainerY const& y);
        void run_gnuplot();
        std::string generate_script();
        void quit_gnuplot();
};


/// Draw a line plot
template<class ContainerX, class ContainerY>
void GNUPlot::plot(ContainerX const& x, ContainerY const& y,
        std::string name,
        bool points,
        std::string color,
        int type,
        int weight)
{
    // Add data
    add_points(x, y);

    // Add style
    if (data_ind > 1) plotting_cmds << ", ";
    plotting_cmds << "$data" << data_ind << " with lines" << (points ? "p" : "");
    // Line styles
    if (!color.empty())
        plotting_cmds << " lc rgb '" << color << "'";

    if (type > 0)
        plotting_cmds << " lt " << type;

    plotting_cmds << " lw " << weight;

    // Add name
    if (!name.empty())
        plotting_cmds << " title \"" << name << "\"";
}

/// Draw a scatter plot
template<class ContainerX, class ContainerY>
void GNUPlot::scatter(ContainerX const& x, ContainerY const& y,
        std::string name,
        std::string style)
{
    // Add points
    add_points(x, y);

    // Add style
    if (data_ind > 1) plotting_cmds << ", ";
    plotting_cmds << "$data" << data_ind << " with points";

    // Add name
    if (!name.empty())
        plotting_cmds << " title \"" << name << "\"";
}

/// Display the current plot
void GNUPlot::show()
{
    run_gnuplot();

    do {
        std::cout << "Press enter to finish. ";
    } while (std::cin.get() != '\n');

    quit_gnuplot();
}

/// Add datapoints under the variable assigned by data_ind
template<class ContainerX, class ContainerY>
void GNUPlot::add_points(ContainerX const& x, ContainerY const& y)
{
    data_ind++;
    plotting_data << "\n$data" << data_ind << " << EOD\n";

    auto x_iter = x.begin();
    auto y_iter = y.begin();
    while (x_iter != x.end() || y_iter != y.end()) {
        plotting_data << *x_iter << " " << *y_iter << "\n";
        x_iter++;
        y_iter++;
    }
    plotting_data << "EOD\n\n";
}

std::string GNUPlot::generate_script()
{
    std::stringstream script_strm;

    script_strm << "\nset title \"" << title << "\"\n";
    script_strm << "set xlabel \"" << xlabel << "\"\n";
    script_strm << "set ylabel \"" << ylabel << "\"\n";
    script_strm << "set autoscale\n";
    script_strm << plotting_data.str() << "\n";
    script_strm << "plot " << plotting_cmds.str() << "\n";

    return script_strm.str();
}

void GNUPlot::run_gnuplot()
{
    plot_process = popen("gnuplot", "w");

    std::string input = generate_script();

    fprintf(plot_process, "%s", input.c_str());
    fflush(plot_process);
    printf("Running:\n\n%s", input.c_str());

}

void GNUPlot::quit_gnuplot()
{
    int code = pclose(plot_process);

    if (code != 0) fprintf(stderr, "GNUPLOT finished with exitcode %d\n", code);
}
