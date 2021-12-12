#pragma once
#include <list>

namespace checkers {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	const int mapSize = 8;
	const int cellSize = 60;

	int** map;

	bool toShowSteps;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	private:
		System::ComponentModel::Container^ components;

		Image^ whiteFigure;
		Image^ blackFigure;
		Image^ whiteDamka;
		Image^ blackDamka;

		Button^ pressedButton;
		Button^ prevButton;

		array<Button^, 2>^ buttons = gcnew array<Button^, 2>(mapSize, mapSize);
		Generic::List<Button^>^ simpleSteps = gcnew Generic::List<Button^>;
		
		int currentPlayer;
		int cntAte = 0; // количество съеденных шашек

		bool isMoving;
		bool isContinue; // можно ли продолжать бить шашки
		
	public:
		MyForm(void)
		{
			InitializeComponent();
			this->Text = "Курсач";
			this->BackColor = Color::Black;
			this->Width = mapSize * cellSize + 20;
			this->Height = mapSize * cellSize + 70;
			Init();
		}

		void Init() {

			whiteFigure = gcnew Bitmap(gcnew Bitmap(L"image\\w.png"), cellSize - 11, cellSize - 11);
			blackFigure = gcnew Bitmap(gcnew Bitmap(L"image\\b.png"), cellSize - 11, cellSize - 11);
			whiteDamka = gcnew Bitmap(gcnew Bitmap(L"image\\wd.png"), cellSize - 11, cellSize - 11);
			blackDamka = gcnew Bitmap(gcnew Bitmap(L"image\\bd.png"), cellSize - 11, cellSize - 11);
			/*whiteFigure = Image::FromFile("D:\\my own programs\\checkers\\image\\w.png");
			blackFigure = Image::FromFile("D:\\my own programs\\checkers\\image\\b.png");
			whiteDamka = Image::FromFile("D:\\my own programs\\checkers\\image\\wd.png");
			blackDamka = Image::FromFile("D:\\my own programs\\checkers\\image\\bd.png");*/

			currentPlayer = 1;
			isMoving = false;
			pressedButton = nullptr;
			toShowSteps = false;

			MenuStrip^ menuStrip = gcnew MenuStrip();
			ToolStripButton^ reset = gcnew ToolStripButton("Новая игра");
			ToolStripButton^ showMe = gcnew ToolStripButton("Показывать хода");
			ToolStripButton^ wtf = gcnew ToolStripButton("О программе...");
			menuStrip->Dock = DockStyle::Top;
			this->Controls->Add(menuStrip);
			menuStrip->Items->Add(reset);
			menuStrip->Items->Add(showMe);
			menuStrip->Items->Add(wtf);
			reset->Click += gcnew EventHandler(this, &MyForm::resett);
			showMe->Click += gcnew EventHandler(this, &MyForm::showMee);
			wtf->Click += gcnew EventHandler(this, &MyForm::wtff);

			map = new int* [mapSize];
			for (size_t i = 0; i < mapSize; i++)
			{
				map[i] = new int[mapSize];
			}
			for (size_t i = 0; i < mapSize; i++)
			{
				for (size_t j = 0; j < mapSize; j++)
				{
					if (i == 0 && j % 2 == 0) map[i][j] = 2;
					else if (i == 1 && j % 2 != 0) map[i][j] = 2;
					else if (i == 2 && j % 2 == 0) map[i][j] = 2;
					else if (i == 5 && j % 2 != 0) map[i][j] = 1;
					else if (i == 6 && j % 2 == 0) map[i][j] = 1;
					else if (i == 7 && j % 2 != 0) map[i][j] = 1;
					else map[i][j] = 0;
				}
			}
			CreateMap();
		}

		System::Void resett(System::Object^ sender, System::EventArgs^ e) {
			reset();
		}
		System::Void showMee(System::Object^ sender, System::EventArgs^ e) {
			toShowSteps = toShowSteps == true ? false : true;
		}
		System::Void wtff(System::Object^ sender, System::EventArgs^ e) {
			MessageBox::Show("Поставьте 100 плз :)", "Артём Геннадьевич!");
		}
		
		void CreateMap() {
			for (int i = 0; i < mapSize; i++)
			{
				for (int j = 0; j < mapSize; j++)
				{
					Button^ button = gcnew Button();
					button->Location = System::Drawing::Point(j * cellSize, i * cellSize + 25);
					button->Size = System::Drawing::Size(cellSize, cellSize);
					button->Visible = true;
					button->FlatStyle = FlatStyle::Flat;
					button->Click += gcnew EventHandler(this, &MyForm::Pressed);

					if (map[i][j] == 1) {
						button->Image = whiteFigure;
					}
					else if (map[i][j] == 2) {
						button->Image = blackFigure;
					}

					this->Controls->Add(button);
					buttons[i, j] = button;

					button->BackColor = getTrueColor(button);
				}
			}
		}

		Color getTrueColor(Button^ button) {   // возвращает изначальный цвет кнопки
			if (button->Location.Y / cellSize % 2 != 0 && button->Location.X / cellSize % 2 == 0) {
				return Color::DarkGray;
			}
			else if (button->Location.Y / cellSize % 2 == 0 && button->Location.X / cellSize % 2 != 0) {
				return Color::DarkGray;
			}
			else {
				return Color::White;
			}
		}

		System::Void Pressed(System::Object^ sender, System::EventArgs^ e) {
			if (prevButton != nullptr) {
				prevButton->BackColor = getTrueColor(prevButton);
			}

			pressedButton = safe_cast<Button^> (sender);

			if (map[pressedButton->Location.Y / cellSize][pressedButton->Location.X / cellSize] != 0 && map[pressedButton->Location.Y / cellSize][pressedButton->Location.X / cellSize] == currentPlayer) {
				closeSteps();
				if (toShowSteps) pressedButton->BackColor = Color::DarkRed;
				disableAllButtons();
				pressedButton->Enabled = true;
				cntAte = 0;
				//if (pressedButton->Text == "D") showSteps(pressedButton->Location.Y / cellSize, pressedButton->Location.X / cellSize, true);
				if (pressedButton->Image == whiteDamka || pressedButton->Image == blackDamka) showSteps(pressedButton->Location.Y / cellSize, pressedButton->Location.X / cellSize, true);
				else showSteps(pressedButton->Location.Y / cellSize, pressedButton->Location.X / cellSize, false);
				
				if (isMoving) {
					closeSteps();
					pressedButton->BackColor = getTrueColor(pressedButton);
					showPossibleSteps();
					isMoving = false;
				}
				else isMoving = true;
			}
			else {
				if (isMoving) {
					isContinue = false;
					if (Math::Abs(pressedButton->Location.X / cellSize - prevButton->Location.X / cellSize) > 1) {
						isContinue = true;
						deleteEaten(pressedButton, prevButton);
					}
					int temp = map[pressedButton->Location.Y / cellSize][pressedButton->Location.X / cellSize];
					map[pressedButton->Location.Y / cellSize][pressedButton->Location.X / cellSize] = map[prevButton->Location.Y / cellSize][prevButton->Location.X / cellSize];
					map[prevButton->Location.Y / cellSize][prevButton->Location.X / cellSize] = temp;
					pressedButton->Image = prevButton->Image;
					prevButton->Image = nullptr;
					pressedButton->Text = prevButton->Text;
					prevButton->Text = "";
					switchButtonToDamka(pressedButton);
					cntAte = 0;
					isMoving = false;
					closeSteps();
					disableAllButtons();
					//if (pressedButton->Text == "D") showSteps(pressedButton->Location.Y / cellSize, pressedButton->Location.X / cellSize, true);
					if (pressedButton->Image == whiteDamka || pressedButton->Image == blackDamka) showSteps(pressedButton->Location.Y / cellSize, pressedButton->Location.X / cellSize, true);
					else showSteps(pressedButton->Location.Y / cellSize, pressedButton->Location.X / cellSize, false);
					if (cntAte == 0 || !isContinue) {
						closeSteps();
						switchPlayer();
						showPossibleSteps();
						isContinue = false;
					}
					else if (isContinue) {
						if (toShowSteps) pressedButton->BackColor = Color::DarkRed;
						pressedButton->Enabled = true;
						isMoving = true;
					}
				}
			}
			prevButton = pressedButton; // кнопка, нажатая сейчас, станет предыдущей для следующей
		}

		void reset() {
			this->Controls->Clear();
			Init();
		}

		void switchPlayer() {
			currentPlayer = currentPlayer == 1 ? 2 : 1;
		} 

		bool isInsideBorders(int i, int j) {
			if (i >= mapSize || j >= mapSize || i < 0 || j < 0) {
				return false;
			}
			return true;
		}

		void enableAllButtons() {
			for (int i = 0; i < mapSize; i++)
			{
				for (int j = 0; j < mapSize; j++)
				{
					buttons[i, j]->Enabled = true;
				}
			}
		}

		void disableAllButtons() {
			for (int i = 0; i < mapSize; i++)
			{
				for (int j = 0; j < mapSize; j++)
				{
					buttons[i, j]->Enabled = false;
				}
			}
		}
		
		void closeSteps() { // возвращает все цвета всей доски в изначальные
			for (int i = 0; i < mapSize; i++)
			{
				for (int j = 0; j < mapSize; j++)
				{
					buttons[i, j]->BackColor = getTrueColor(buttons[i, j]);
				}
			}
		}

		void showPossibleSteps() {
			bool isDamka = false;
			bool isEatStep = false; // есть ли съедобные ходы или нет
			disableAllButtons();
			for (size_t i = 0; i < mapSize; i++)
			{
				for (size_t j = 0; j < mapSize; j++)
				{
					if (map[i][j] == currentPlayer) {
						//if (buttons[i, j]->Text == "D") isDamka = true;
						if (pressedButton->Image == whiteDamka || pressedButton->Image == blackDamka) isDamka = true;
						else isDamka = false;
						if (isAbleToEat(i, j, isDamka, new int[2]{ 0, 0 })) {
							isEatStep = true;
							buttons[i, j]->Enabled = true;
						}
					}
				}
			}
			if (!isEatStep) enableAllButtons();
		}

		void switchButtonToDamka(Button^ button) {
			if (map[button->Location.Y / cellSize][button->Location.X / cellSize] == 2 && button->Location.Y / cellSize == mapSize - 1) {
				//button->Text = "D";
				//button->BackgroundImage = nullptr;
				button->Image = blackDamka;
			}
			if (map[button->Location.Y / cellSize][button->Location.X / cellSize] == 1 && button->Location.Y / cellSize == 0) {
				//button->Text = "D";
				//button->BackgroundImage = nullptr;
				button->Image = whiteDamka;
			}
		}

		void deleteEaten(Button^ endButton, Button^ startButton) {
			int count = Math::Abs(endButton->Location.Y / cellSize - startButton->Location.Y / cellSize);
			int startIndexX = endButton->Location.Y / cellSize - startButton->Location.Y / cellSize;
			int startIndexY = endButton->Location.X / cellSize - startButton->Location.X / cellSize;
			startIndexX = startIndexX < 0 ? -1 : 1;
			startIndexY = startIndexY < 0 ? -1 : 1;
			int currCount = 0;
			int i = startButton->Location.Y / cellSize + startIndexX;
			int j = startButton->Location.X / cellSize + startIndexY;
			while (currCount < count - 1) {
				map[i][j] = 0;
				buttons[i, j]->Image = nullptr;
				buttons[i, j]->Text = "";
				i += startIndexX;
				j += startIndexY;
				currCount++;
			}
		}

		void showSteps(int currentI, int currentJ, bool isDamka) {
			simpleSteps->Clear();
			showDiagonal(currentI, currentJ, isDamka);
			if (cntAte > 0) closeSimpleSteps(simpleSteps);
		}

		void showDiagonal(int currentI, int currentJ, bool isDamka) {
			int j = currentJ + 1;
			for (int i = currentI - 1; i >= 0; i--) {
				if (currentPlayer == 2 && !isDamka && !isContinue) break;
				if (isInsideBorders(i, j)) {
					if (!determinePath(i, j)) break;
				}
				if (j < 7) j++;
				else break;
				if (!isDamka) break;
			}

			j = currentJ - 1;
			for (int i = currentI - 1; i >= 0; i--) {
				if (currentPlayer == 2 && !isDamka && !isContinue) break;
				if (isInsideBorders(i, j)) {
					if (!determinePath(i, j)) break;
				}
				if (j > 0) j--;
				else break;
				if (!isDamka) break;
			}

			j = currentJ - 1;
			for (int i = currentI + 1; i < 8; i++) {
				if (currentPlayer == 1 && !isDamka && !isContinue) break;
				if (isInsideBorders(i, j)) {
					if (!determinePath(i, j)) break;
				}
				if (j > 0) j--;
				else break;
				if (!isDamka) break;
			}

			j = currentJ + 1;
			for (int i = currentI + 1; i < 8; i++) {
				if (currentPlayer == 1 && !isDamka && !isContinue) break;
				if (isInsideBorders(i, j)) {
					if (!determinePath(i, j)) break;
				}
				if (j < 7) j++;
				else break;
				if (!isDamka) break;
			}
		}

		bool determinePath(int i, int j) {
			if (map[i][j] == 0 && !isContinue) {
				if (toShowSteps) buttons[i, j]->BackColor = Color::Yellow;
				buttons[i, j]->Enabled = true;
				simpleSteps->Add(buttons[i, j]);
			}
			else {
				if (map[i][j] != currentPlayer) {
					//if (pressedButton->Text == "D") showProceduralEat(i, j, true);
					if (pressedButton->Image == whiteDamka || pressedButton->Image == blackDamka) showProceduralEat(i, j, true);
					else showProceduralEat(i, j, false);
				}
				return false;
			}
			return true;
		}

		void closeSimpleSteps(Generic::List<Button^>^ simpleSteps) {
			if (simpleSteps->Count > 0) {
				for (size_t i = 0; i < simpleSteps->Count; i++)
				{
					simpleSteps[i]->BackColor = getTrueColor(simpleSteps[i]);
					simpleSteps[i]->Enabled = false;
				}
			}
		}

		void showProceduralEat(int i, int j, bool isDamka) {
			int dirX = i - pressedButton->Location.Y / cellSize;
			int dirY = j - pressedButton->Location.X / cellSize;
			dirX = dirX < 0 ? -1 : 1;
			dirY = dirY < 0 ? -1 : 1;
			int il = i;
			int jl = j;
			bool isEmpty = true;
			while (isInsideBorders(il, jl)) {
				if (map[il][jl] != 0 && map[il][jl] != currentPlayer) {
					isEmpty = false;
					break;
				}
				il += dirX;
				jl += dirY;
				if (!isDamka) break;
			}
			if (isEmpty) return;
			
			Generic::List<Button^>^ toClose = gcnew Generic::List<Button^>;
			bool closeSimple = false;

			int ik = il + dirX;
			int jk = jl + dirY;
			while (isInsideBorders(ik, jk)) {
				if (map[ik][jk] == 0) {
					if (isAbleToEat(ik, jk, isDamka, new int[2]{ dirX, dirY })) {
						closeSimple = true;
					}
					else {
						toClose->Add(buttons[ik, jk]);
					}
					if (toShowSteps) buttons[ik, jk]->BackColor = Color::Yellow;
					buttons[ik, jk]->Enabled = true;
					cntAte++;
				}
				else break;
				if (!isDamka) break;
				ik += dirX;
				jk += dirY;
			}
			if (closeSimple && toClose->Count > 0) {
				closeSimpleSteps(toClose);
			}
		}

		bool isAbleToEat(int currentI, int currentJ, bool isDamka, int direction[]) {
			bool eatStep = false;
			
			int j = currentJ + 1;
			for (int i = currentI - 1; i >= 0; i--) // проверка вправо вверх
			{
				if (currentPlayer == 2 && !isDamka && !isContinue) break; // отключение проверки в этом направлении если текущий игрок верхний, недамка, первый ход на битьё 
				if (direction[0] == 1 && direction[1] == -1 && isDamka) break; // отключение проверки на направления с которого пришла шашка
				if (isInsideBorders(i, j)) {
					if (map[i][j] != 0 && map[i][j] != currentPlayer) {
						eatStep = true;
						if (!isInsideBorders(i - 1, j + 1)) eatStep = false; // если после потенциального битья мы выйдем за границу то бить не сможем
						else if (map[i - 1][j + 1] != 0) eatStep = false; // если после потенциальной шашки в этом же направлении есть другая шашка то побить не сможем
						else return eatStep;
					}
				}
				if (j < 7) j++;
				else break;
				if (!isDamka) break;
			}
			
			j = currentJ - 1;
			for (int i = currentI - 1; i >= 0; i--) // проверка влево вверх
			{
				if (currentPlayer == 2 && !isDamka && !isContinue) break; // отключение проверки в этом направлении если текущий игрок верхний, недамка, первый ход на битьё 
				if (direction[0] == 1 && direction[1] == 1 && isDamka) break; 
				if (isInsideBorders(i, j)) {
					if (map[i][j] != 0 && map[i][j] != currentPlayer) {
						eatStep = true;
						if (!isInsideBorders(i - 1, j - 1)) eatStep = false; // если после потенциального битья мы выйдем за границу то бить не сможем
						else if (map[i - 1][j - 1] != 0) eatStep = false; // если после потенциальной шашки в этом же направлении есть другая шашка то побить не сможем
						else return eatStep;
					}
				}
				if (j > 0) j--;
				else break;
				if (!isDamka) break;
			}

			j = currentJ - 1;
			for (int i = currentI + 1; i >= 0; i--) // проверка влево вниз
			{
				if (currentPlayer == 1 && !isDamka && !isContinue) break; // отключение проверки в этом направлении если текущий игрок нижний, недамка, первый ход на битьё 
				if (direction[0] == -1 && direction[1] == 1 && isDamka) break;
				if (isInsideBorders(i, j)) {
					if (map[i][j] != 0 && map[i][j] != currentPlayer) {
						eatStep = true;
						if (!isInsideBorders(i + 1, j - 1)) eatStep = false; // если после потенциального битья мы выйдем за границу то бить не сможем
						else if (map[i + 1][j - 1] != 0) eatStep = false; // если после потенциальной шашки в этом же направлении есть другая шашка то побить не сможем
						else return eatStep;
					}
				}
				if (j > 0) j--;
				else break;
				if (!isDamka) break;
			}

			j = currentJ + 1;
			for (int i = currentI + 1; i >= 0; i--) // проверка вправо вниз
			{
				if (currentPlayer == 1 && !isDamka && !isContinue) break; // отключение проверки в этом направлении если текущий игрок нижний, недамка, первый ход на битьё 
				if (direction[0] == -1 && direction[1] == -1 && isDamka) break;
				if (isInsideBorders(i, j)) {
					if (map[i][j] != 0 && map[i][j] != currentPlayer) {
						eatStep = true;
						if (!isInsideBorders(i + 1, j + 1)) eatStep = false; // если после потенциального битья мы выйдем за границу то бить не сможем
						else if (map[i + 1][j + 1] != 0) eatStep = false; // если после потенциальной шашки в этом же направлении есть другая шашка то побить не сможем
						else return eatStep;
					}
				}
				if (j < 7) j++;
				else break;
				if (!isDamka) break;
			}
			return eatStep;
		}

	protected:
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}






#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->SuspendLayout();
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(279, 261);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}