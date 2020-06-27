#include "poissondisk.h"
#include "display.h"
#include <iostream>
using namespace std;
std::vector<Point> samples;
std::vector<std::vector<float>> powerSpectrum;
PoissonDisk pd;
void testRInput() {
	int w = 540, h = 540, s = 540;
	for (int r = 2; r <= 6; r++) {
		pd.fastPoissonDiskSampling(samples, r, w, h, 20);
		getFourierPowerSpectrum(powerSpectrum, s, pd.getWidth(), pd.getHeight(), samples);
		string cfg = "_r_" + to_string(r)
			+ "_N_" + to_string(samples.size())
			+ "_d_" + to_string(getDiscrepancy(w, h, samples));
		display(pd.getWidth(), pd.getHeight(), samples, "BNS" + cfg, true);
		display(powerSpectrum, "FSA" + cfg, true);
	}
}
void testNumInput() {
	std::vector<int> nums = { 500,1000,2000,4000,8000,16000 };
	for (auto& i : nums) {
		pd.fastPoissonDiskSampling(samples, i);
		int s = (pd.getWidth() + pd.getHeight()) / 8;
		getFourierPowerSpectrum(powerSpectrum, s, pd.getWidth(), pd.getHeight(), samples);
		string cfg = "_r_" + to_string(pd.getR())
			+ "_N_" + to_string(samples.size())
			+ "_d_" + to_string(getDiscrepancy(pd.getWidth(), pd.getHeight(), samples));
		display(pd.getWidth(), pd.getHeight(), samples, "BNS" + cfg, true);
		display(powerSpectrum, "FSA" + cfg, true);
	}
}
void interact() {
	int n;
	std::vector<Point> samples;
	cout << "建议2000<=n<10000\n>>> N=";
	while (cin >> n) {
		pd.fastPoissonDiskSampling(samples, n);
		cout << "accurate N=" << samples.size() << endl;
		display(pd.getWidth(), pd.getHeight(), samples,"BlueNoiseSampling",false);
		if (n < 2000) {
			cout << "N<2000时，精度不足，傅里叶能量谱不会被计算\n>>> N=";
			continue;
		}
		else if (n > 10000) {
			cout << "N>10000时，耗时过长，傅里叶能量谱不会被计算\n>>> N=";
			continue;
		}
		int s = (pd.getWidth() + pd.getHeight()) / 8;
		getFourierPowerSpectrum(powerSpectrum, s, pd.getWidth(), pd.getHeight(), samples);
		display(powerSpectrum, "FourierSpectrumAnalysis", false);
		cout << "Discrepancy=" << getDiscrepancy(pd.getWidth(), pd.getHeight(), samples) << endl;
		cout << ">>> N=";
	}
}
void findRelationBetweenNumAndSize();
int main() {
	//testNumInput();
	//testRInput();

	interact();

	//findRelationBetweenNumAndSize();

	return 0;
}
void findRelationBetweenNumAndSize() {
	//输出一段matlab代码，拟合采样点数量和采样区域面积的关系
	PoissonDisk pd;
	string x = "x=[", y = "y=[";;
	vector<Point> samples;
	for (int w = 20; w < 180; w++) {
		pd.fastPoissonDiskSampling(samples, 8, w, w, 20);
		x.append(to_string(samples.size()) + ",");
		y.append(to_string(w * w) + ",");
	}
	x.append("];");
	y.append("];");
	string query = x + "\n" + y +
		"\nC=polyfit(x,y,2)" +
		"\nZ = polyval(C, x);" +
		"\nplot(x, y, \'r*\', x, Z, \'b\');";
	cout << query << endl;
}