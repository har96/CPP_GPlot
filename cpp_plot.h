#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>


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

        template<class ContainerX, class ContainerY, class ContainerZ>
        void scatter(ContainerX const& x, ContainerY const& y,
                std::string name="",
                ContainerZ const* labels=nullptr,
                std::string label_offset="1,1",
                std::string style="");

        inline void setTitle(std::string title) { this->title = title; };
        inline void setXLabel(std::string xlabel) { this->xlabel = xlabel; };
        inline void setYLabel(std::string ylabel) { this->ylabel = ylabel; };
        inline void setLabelFont(int size) { this->label_font = size; };
        inline void setTitleFont(int size) { this->title_font = size; };

        void show();

    private:
        std::string title;
        std::string xlabel, ylabel;
        int label_font = 0, title_font = 0;

        std::stringstream plotting_data;
        std::stringstream plotting_cmds;

        FILE * plot_process;

        int data_ind;

        template<class ContainerX, class ContainerY, class ContainerZ>
        void add_points(ContainerX const& x, ContainerY const& y, ContainerZ const* z);
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
template<class ContainerX, class ContainerY, class ContainerZ=std::vector<int>>
void GNUPlot::scatter(ContainerX const& x, ContainerY const& y,
        std::string name,
        ContainerZ const* labels,
        std::string label_offset,
        std::string style)
{
    // Add points
    if (labels != nullptr) {
        add_points(x, y, labels);
    } else {
        add_points(x, y);
    }

    // Add style
    if (data_ind > 1) plotting_cmds << ", ";
    plotting_cmds << "$data" << data_ind;
    if (labels)
        plotting_cmds << " with labels point offset char " <<  label_offset;
    else
        plotting_cmds << " with points";

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
template<class ContainerX, class ContainerY, class ContainerZ>
void GNUPlot::add_points(ContainerX const& x, ContainerY const& y, ContainerZ const* labels)
{
    data_ind++;
    plotting_data << "\n$data" << data_ind << " << EOD\n";

    auto x_iter = x.begin();
    auto y_iter = y.begin();
    auto label_iter = labels->begin();
    while (x_iter != x.end() || y_iter != y.end()) {
        plotting_data << *x_iter << " " << *y_iter << " " << *label_iter <<  "\n";
        x_iter++;
        y_iter++;
        label_iter++;
    }
    plotting_data << "EOD\n\n";
}

template<class ContainerX, class ContainerY>
void GNUPlot::add_points(ContainerX const& x, ContainerY const& y)
{
    data_ind++;
    plotting_data << "\n$data" << data_ind << " << EOD\n";

    auto x_iter = x.begin();
    auto y_iter = y.begin();
    while (x_iter != x.end() || y_iter != y.end()) {
        plotting_data << *x_iter << " " << *y_iter <<  "\n";
        x_iter++;
        y_iter++;
    }
    plotting_data << "EOD\n\n";
}

std::string GNUPlot::generate_script()
{
    std::stringstream script_strm;

    script_strm << "\nset title \"" << title << "\"";
    script_strm << (title_font ? " font \"," + std::to_string(title_font) + "\"\n" : "\n");
    script_strm << "set xlabel \"" << xlabel << "\"";
    script_strm << (label_font ? " font \"," + std::to_string(label_font) + "\"\n" : "\n");
    script_strm << "set ylabel \"" << ylabel << "\"";
    script_strm << (label_font ? " font \"," + std::to_string(label_font) + "\"\n" : "\n");
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
