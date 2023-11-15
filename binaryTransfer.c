#include <stdio.h>
#include <math.h>



int dTob(int num) {
    if (num == 0) {
        return 0;
    }

    // 입력된 숫자가 음수일 경우
    if (num < 0) {
        return -1; // 에러 코드, 음수는 처리할 수 없음을 나타냄
    }

    int binary = 0;
    int remainder, i = 1;

    while (num != 0) {
        remainder = num % 2;
        num = num / 2;
        binary += remainder * i;
        i *= 10; // 2진수 자리수 맞추기 위해 10씩 곱해줌
    }

    return binary;
}

int bTod(int binary) {
    int decimal = 0, i = 0, remainder;

    while (binary != 0) {
        remainder = binary % 10;
        binary = binary / 10;
        decimal += remainder * pow(2, i);
        ++i;
    }

    return decimal;
}

// int main() {
	
// 	printf("%d\n\n",  dTob(0xff));
//     int number;
//     printf("10진수 숫자를 입력하세요: ");
//     scanf("%d", &number);

//     int binaryResult = dTob(number);

//     if (binaryResult == -1) {
//         printf("음수는 처리할 수 없습니다.\n");
//     } else {
//         printf("이진수: %d\n", binaryResult);
//     }

    
//     int binaryNumber;
//     printf("이진수를 입력하세요 (최대 32비트): ");
//     scanf("%d", &binaryNumber);

//     int decimalResult = bTod(binaryNumber);
//     printf("십진수로 변환된 값: %d\n", decimalResult);

//     return 0;
// }
