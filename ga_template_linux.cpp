#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define ELITE 1				/* 優先的に残すエリート個体の数 */
#define CROSS_MUTATION 50	/* 交叉・突然変異で生成する個体数（偶数にすること） */
#define IND (CROSS_MUTATION+ELITE) /* 全個体数 */
#define NUM	100			/* 遺伝子長 */
#define CROSSOVER_RATE 0.1	/* 交叉率（交換する遺伝子の割合）*/
#define MUTATION_RATE 0.01	/* 突然変異率（変化する遺伝子の割合）*/
#define GENERATION 100  	/* 世代数 */

/* グローバル変数 */
int generation;
	
/* 進化用の関数 */
void genetic_operator(int gene[][NUM], double fitness[], int elite);

int main() {

	int i,j;
	int gene[IND][NUM];		/* 遺伝子配列 INDは個体数，NUMは各個体の遺伝子数 */
	double fitness[IND];	/* 適応度（個体数分用意する）*/
	double elite_fitness,worst_fitness,average;	/* エリートの適応度，最悪個体の適応度，平均適応度 */
	int elite,worst;	/* エリートの個体番号，最悪個体の個体番号 */
	double sum = 0.0;
	srand((unsigned int)time(NULL));		/*乱数を現在時刻で初期化*/

	FILE* f1 = fopen("elite.txt", "w");
	FILE* f2 = fopen("worst.txt", "w");
	FILE* f3 = fopen("average.txt", "w");
	FILE* f4 = fopen("last_data.txt", "w");

	/* 手順1　すべての個体の遺伝子の初期化（0か1でランダムに初期化する）*/
	for(i=0; i < IND; i++){
		for(j=0; j < NUM; j++){
			gene[i][j] = rand() % 2;
		}
	}

	/* 世代ループ */
	for (generation=0; generation<GENERATION; generation++) {

		/* 手順2-1 適応度計算 */
		for (i=0; i < IND; i++) {
			fitness[i] = 0;
			for(int j=0; j < NUM; j++){
				fitness[i] += gene[i][j];
			}
		}
		
		/* 手順2-2 結果表示用に、1) エリート（最良個体）の適応度と個体番号を記録し、2)最悪個体の適合度と個体番号を記録し、3) 全個体の平均適応度を記録する */
		average=0.0;	/* 平均適応度の初期化 */
		elite_fitness=-1.0;	/* エリートの適応度の初期化 */
		worst_fitness=(double)NUM+1.0;	/* 最悪個体の適応度の初期化 */
		elite = 0;	/* エリートの個体番号の初期化 */
		worst = 0;	/* 最悪個体の個体番号の初期化 */

		i = 1;
		elite_fitness = fitness[0]; 
		while(fitness[i]){
			if(elite_fitness < fitness[i]){
				elite_fitness = fitness[i];
				elite = i;
			}
			i++;
		}

		i = 1;
		worst_fitness = fitness[0]; 
		while(fitness[i]){
			if(worst_fitness > fitness[i]){
				worst_fitness = fitness[i];
				worst = i;
			}
			i++;
		}

		i=0;
		while(fitness[i]){
			sum += fitness[i];
			i++;
		}
		average = sum / IND;
		sum = 0.0;

		/* 1)～3)で計算した値を表示する */
		printf("generation = %d		elite = %lf		ave = %lf		worst = %lf", generation+1, elite_fitness, average, worst_fitness);
		printf("\n");
		fprintf(f1, "%lf\n", elite_fitness);
		fprintf(f2, "%lf\n", worst_fitness);
		fprintf(f3, "%lf\n", average);

		/* 手順3 進化（選択・交叉・突然変異）main関数下に作成しているgenetic_operator関数内に記述すること */
		genetic_operator(gene,fitness, elite);
	}

	/* 手順4 最終世代の最良個体の遺伝子をfprintfで保存 */
	fprintf(f4, "%lf\n", elite_fitness);

	return EXIT_SUCCESS;

}

void genetic_operator(int gene[][NUM], double fitness[], int elite) {

	int i,j;
	int gene_temp[IND][NUM];	/* 新しく生成された個体は，臨時の配列にまず保存する */
	int candidate[2];			/* トーナメント選択実行中に戦う2個体の番号保存用（使用は任意） */
	int parent[2];				/* トーナメント選択を2回行った結果選ばれた親個体番号保存用（使用は任意） */

	/* 選択・交叉・突然変異 */
	
	/* 手順3-1 エリート選択（エリート1個体を臨時配列にコピーすること）
	（発展課題：適応度の高い順に複数個のエリート個体を残したい場合どうしますか？）*/
	for(i = 0; i < NUM; i++){
		gene_temp[IND-1][i] = gene[elite][i];
	}

	for (int i = 0; i < CROSS_MUTATION/2; i++) { /* たとえば、CROSS_MUTATION=50のとき、交叉を50の半分の25回繰り返せば、50個の子個体が生成できる */
		
		/* 手順3-2 交叉における選択：トーナメント選択を2回行い、2個の親個体を選出する */
		for (j = 0; j < 2; j++) {	/* トーナメント選択を2回すれば2個体が選出できる */
			for(int k = 0; k < 2; k++){
				candidate[k] = rand() % IND;
				//printf("%d\n", candidate[k]);
			}
			parent[j] = candidate[0];
			//printf("%lf\n", fitness[candidate[0]]);
			if(fitness[candidate[0]] < fitness[candidate[1]]){
				parent[j] = candidate[1];
				//printf("%d\n", parent[j]);
			}
			for(int k = 0; k < NUM; k++){
				gene_temp[2*i+j][k] = gene[parent[j]][k];
				//printf("%d\n", gene_temp[2*i+j][k]);
			}
		}
		/* 手順3-3 交叉：2個体間の交叉を実行する（各遺伝子を交叉率にしたがって交換する．一様交叉）*/
		int temp;
		for(j = 0; j < NUM; j++){
			if((double)rand()/RAND_MAX < CROSSOVER_RATE){
				/*親子体間で遺伝子の値を入れ替える*/
				temp = gene_temp[2*i][j];
				gene_temp[2*i][j] = gene[2*i+1][j];
				gene[2*i+1][j] = temp;
			}
		}
	}

	/* 手順3-4 突然変異 */
	for (i=0; i<CROSS_MUTATION; i++) {
		for (j=0; j<NUM; j++) {
			/* 各遺伝子について，突然変異確率で選択されれば，0→1，1→0に変更する */
			if((double)rand()/RAND_MAX < MUTATION_RATE){
				if(gene_temp[i][j] == 1){
					gene_temp[i][j] = 0;
				}else{
					assert(gene_temp[i][j] == 0);
					gene_temp[i][j] = 1;
				}
			}
		}
	}

	/* 新しい個体の生成が終わったら，親個体の配列を子個体の臨時配列で全て置き換える（1世代終了）*/
	for (i=0; i<IND; i++) {
		for (j=0; j<NUM; j++) {
			gene[i][j]=gene_temp[i][j];
		}
	}
}