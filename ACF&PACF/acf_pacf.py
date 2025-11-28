import numpy as np
import pandas as pd
import os
import matplotlib.pyplot as plt


# ========== Problem 1 ========== #
# ---------- MA(2) ----=----- #
# x_t = a_t + 0.7a_(t-1) + 0.8a_(t-2)
def MA(n, theta1, theta2, mu=0, sigma=1):
    # 1) White Gaussian 분포에 따른 랜덤 샘플
    at = np.random.normal(0, sigma, n)  # White Gaussian Noise (a_t)

    # 2) 시계열 데이터를 저장할 배열 초기화
    x_MA = np.zeros(n)

    # 3) MA 공식 적용
    for t in range(n):
        # 관측 전(t<0)에는 노이즈에 대한 정보가 없으므로 0으로 가정
        # t=0 일 때는
        if t == 0:
            # a_{t-1}, a_{t-2} 가 0이라고 가정
            x_MA[t] = at[t] + mu
        # t=1 일 때
        elif t == 1:
            # a_{t-2} 가 0이라고 가정
            x_MA[t] = at[t] + theta1 * at[t - 1] + mu
        else:
            # t=2 부터는  공식 그대로 적용
            x_MA[t] = at[t] + theta1 * at[t - 1] + theta2 * at[t - 2] + mu

    return x_MA  # MA 결과 반환


# ----------- AR(2) ---------- #
# x_t = 1.5x_(t-1) - 0.75x_(t-2}=) + a_t
def AR(n, phi1, phi2, x0, x1, mu=0, sigma=1):
    # 1) White Gaussian 분포에 따른 랜덤 샘플
    at = np.random.normal(0, sigma, n)  # White Gaussian Noise (a_t)

    # 2) 시계열 데이터를 저장할 배열 초기화
    x_AR = np.zeros(n)

    # 3) AR(2) 모델 초기값 설정
    x_AR[0] = x0
    x_AR[1] = x1

    # 4) AR(2) 공식 적용
    # x_0, x_1은 이미 초기값이 설정되어 있기 떄문에, t=1부터 순회
    for t in range(2, n):
        x_AR[t] = phi1 * x_AR[t - 1] + phi2 * x_AR[t - 2] + at[t] + mu

    return x_AR  # AR 결과 반환


# ========== Problem 2/3 ========== #


# ---------- ACF ---------- #
# 입력 시계열 데이터에 대해서 lag=0~lag까지 Autocorrelation을 계산하는 함수
def ACF(data, lag):
    # 1) 입력은 배열로 변환
    data = np.asarray(data)
    n = len(data)

    # 2) ACF 값을 저장할 리스트 초기화
    acf_vals = []

    # 3) Lag 0 부터 최대 Lag까지 순회
    for h in range(0, lag + 1):
        # 3-1) h만큼 떨어진 두 시계열 데이터 생성
        Xt = data[h:]  # x_t
        Xt_h = data[:-h]  # x_{t-h}

        # 3-2) lag=0인 경우
        if h == 0:
            # Corr(x_t, x_t)=1
            # 자기 자신과의 상관관계는 항상 1
            acf_vals.append(1.0)  # 1.0 추가
            continue  # 다음 루프로 이동
        # 3-3) lag>0인 경우
        else:
            # 상관계수 계산
            corr_matrix = np.corrcoef(Xt, Xt_h)  # 2X2 행렬이 생성
            corr = corr_matrix[0, 1]  # (0, 1) 위치 값을 가져옴

            acf_vals.append(corr)  # 상관관계 값을 리스트에 추가

    # 결과 리스트를 배열로 변환하여 반환
    return np.array(acf_vals)


# 계산돈 ACF 값을 받아 stem plot으로 시각화 하는 함수
def plot_ACF(acf_vals, lag, n, title):
    # 1) 95% 신뢰구간 계산
    # - 해당 구간을 벗어나면 통계적으로 유의미하다고 판단함
    conf_interval = 1.96 / np.sqrt(n)

    # 2) plot할 데이터 준비
    lags = range(1, lag + 1)  # lag 1부터 플롯 (x축)
    acf_plot = acf_vals[1 : lag + 1]  # lag 0 (값 1) 제외 (y축)

    # 3) fig 생성 (가로:10,세로:8)
    plt.figure(figsize=(10, 8))

    # 4) stem plot 생성
    plt.stem(lags, acf_plot)
    plt.title(f"Autocorrelation Function (ACF) - {title}")  # 제목 설정

    # 5) 95% 신뢰구간 표시 (경계선 표시)
    # 중심선
    plt.axhline(0, color="blue", linewidth=1)
    # 상한선
    plt.axhline(
        conf_interval,
        color="blue",
        linestyle="--",
        linewidth=1,
        label="95% Confidence Interval",
    )
    # 하한선
    plt.axhline(-conf_interval, color="blue", linestyle="--", linewidth=1)

    # 6) 축/라벨/범례/그리드 설정
    plt.xlabel("Lag")
    plt.ylabel("ACF")
    plt.xlim(0, lag + 1)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()


# ---------- PACF ---------- #
# 입력 시계열 데이터에 대해서 lag=0~lag까지 PACF를 계산하는 함수
def PACF(data, lag):
    # 1) 입력 데이터를 배열로 변환
    x = np.asarray(data, dtype=float)

    # 2) 데이터 중심화 (0 중심으로)
    # - 회귀 분석 시 절편을 고려하지 않고 계산 가능
    x = x - x.mean()
    n = len(x)

    # 3) PACF 값을 저장할 리스트 초기화
    pacf_vals = []

    # 4) lag 0부터 최대 lag까지 순회
    for h in range(0, lag + 1):
        # 4-1) lag=0인 경우
        if h == 0:
            pacf_vals.append(1.0)  # 1.0 추가
            continue  # 다음 루프로 이동
        # 4-2) lag=1인 경우
        elif h == 1:
            # lag가 1인 경우에는 PACF는 중간에 낀 값이 없음
            # PACF(1)=ACF(1)임을 알 수 있음
            Xt = x[1:]  # x_t
            Xt_h = x[:-1]  # x_{t-h}
            phi = np.corrcoef(Xt, Xt_h)[0, 1]
            pacf_vals.append(phi)
            continue
        # 4-3) lag>0인 경우
        # 5) 회귀 분석 수행
        else:
            # 5-1) 회귀분석에 사용할 변수
            Xt = x[h:]  # [x_h, x_{h+1}, ..., x_{n-1}]
            Xt_h = x[:-h]  # [x_0, x_1, ..., x_{n-h-1}]

            # 5-2) 중간 값을 저장할 변수
            Z = []

            # k가 1~(h-1)까지 순회
            for k in range(1, h):
                Z.append(x[h - k : n - k])  # t시점을 기준으로 (t-k) 시점의 값들

            # 5-3) Z 값을 (n-h)x(h-1) 행렬로 변환
            # - 각 열이 x_{t-1}, x_{t-2}, ... 의 값이 됨
            Z = np.column_stack(Z)

            # 5-4) Xt를 중간 값(Z)으로 회귀
            # - 최소제곱법을 수행
            # alpha, beta = (Z*alpha) 혹은 (Z*beta)가 Xt와 가장 비슷해지는 계수
            alpha_Xt = np.linalg.lstsq(Z, Xt, rcond=None)[0]
            beta_Xth = np.linalg.lstsq(Z, Xt_h, rcond=None)[0]

            # 5-5) 잔차(Error) 계산
            # error = 실제값 - 예측값
            err_Xt = Xt - Z @ alpha_Xt
            err_Xth = Xt_h - Z @ beta_Xth

            # 5-6) PACF(h) 값 계산
            # - 두 잔차의 상관관계를 구하는 것이 곧 Xt와 Xth간의 직접적인 상관계수를 계산하는 것과 동일한 의미를 가짐
            phi_h = np.corrcoef(err_Xt, err_Xth)[0, 1]
            pacf_vals.append(phi_h)

    # 결과 리스트를 배열로 변환하여 반환
    return np.array(pacf_vals)


# 계산돈 PACF 값을 받아 stem plot으로 시각화 하는 함수
def plot_PACF(pacf_vals, lag, n, title):
    # 1) 95% 신뢰구간 계산
    # - 해당 구간을 벗어나면 통계적으로 유의미하다고 판단함
    conf_interval = 1.96 / np.sqrt(n)

    # 2) plot할 데이터 준비
    lags = range(1, lag + 1)  # lag 1부터 플롯 (x축)
    pacf_plot = pacf_vals[1 : lag + 1]  # lag 0 제외 (y축)

    # 3) fig 생성 (가로:10,세로:8)
    plt.figure(figsize=(10, 8))
    # 4) stem plot 생성
    plt.stem(lags, pacf_plot)
    plt.title(f"Partial Autocorrelation Function (PACF) - {title}")  # 제목 설정

    # 5) 95% 신뢰구간 표시 (경계선 표시)
    # 중심선
    plt.axhline(0, color="blue", linewidth=1)
    # 상한선
    plt.axhline(
        conf_interval,
        color="blue",
        linestyle="--",
        linewidth=1,
        label="95% Confidence Interval",
    )
    # 하한선
    plt.axhline(-conf_interval, color="blue", linestyle="--", linewidth=1)

    # 6) 축/라벨/범례/그리드 설정
    plt.xlabel("Lag")
    plt.ylabel("PACF")
    plt.xlim(0, lag + 1)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()


# ========== Problem 4 ========== #


# 읽은 CSV 파일을 바탕으로 Price column의 ACF/PACF를 계산하고 plot하는 함수
def run_csv(CSV_lag, filename):
    print(f"\n---------- Problem 4 : {filename} Data Analysis ")

    # 1) CSV 파일 읽기
    # - header=0을 통해 파일의 첫 번째 행을 컬럼 이름으로 사용 (Date, Price)
    # - Date라는 컬럼을 날짜/시간으로 자동 변환
    df = pd.read_csv(filename, header=0, parse_dates=["Date"])

    # 2) 데이터 추출
    data = df["Price"].values  # Price 컬럼만 선택해 data 배열에 저장

    # 3) 데이터 길이 저장
    # - confidence interval을 계산하기 위한 데이터 길이 계산
    n = len(data)

    print(f"Analysis Price column")

    # 4) ACF 계산 및 Plotting
    print(f"calculate ACF & Plotting")
    acf_csv = ACF(data, CSV_lag)
    plot_ACF(acf_csv, CSV_lag, n, "Bitcoin Data (Price)")

    # 4) PACF 계산 및 Plotting
    print(f"calculate PACF & Plotting")
    pacf_csv = PACF(data, CSV_lag)
    plot_PACF(pacf_csv, CSV_lag, n, "Bitcoin Data (Price)")


def main():
    N_sample = 1440  # 시계열 데이터 총 개수 1440
    N_lag = 140  # ACF와 PACF를 계산할 최대 lag 140
    np.random.seed(0)  # 코드 실행 시 항상 동일한 white noise 생성

    # ========== Problem 1 ========== #
    # 1-1) MA(2) 파라미터
    MA_THETA1 = 0.7
    MA_THETA2 = 0.8

    # 1-2) AR(2) 파라미터
    AR_PHI1 = 1.5
    AR_PHI2 = -0.75

    # 1-3) AR(2) 초기값
    AR_X0 = 2
    AR_X1 = 3

    # 1-4) MA와 AR 모델 적용
    ma2 = MA(N_sample, MA_THETA1, MA_THETA2)
    ar2 = AR(N_sample, AR_PHI1, AR_PHI2, AR_X0, AR_X1)

    print(f"MA(2) 데이터 (n={N_sample}) 생성 완료")
    print(f"AR(2) 데이터 (n={N_sample}) 생성 완료")

    # 1-5) 시계열 데이터 플로팅 (MA, AR)
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

    # 1-6) MA(2) plot
    ax1.plot(ma2)
    ax1.set_title("MA(2) Time Series Simulation")
    ax1.set_xlabel("t")
    ax1.set_ylabel("x_t")

    # 1-7) AR(2) plot
    ax2.plot(ar2)
    ax2.set_title("AR(2) Time Series Simulation")
    ax2.set_xlabel("t")
    ax2.set_ylabel("x_t")
    plt.tight_layout()
    plt.show()

    # ========== Problem 2/3 ========== #
    # MA(2)와 AR(2)에 대한 ACF/PACF 값 계산
    acf_ma2 = ACF(ma2, N_lag)
    acf_ar2 = ACF(ar2, N_lag)
    pacf_ma2 = PACF(ma2, N_lag)
    pacf_ar2 = PACF(ar2, N_lag)

    print(f"ACF for MA(2) (첫 5개): {acf_ma2[:5]}")
    print(f"ACF for AR(2) (첫 5개): {acf_ar2[:5]}")

    # ACF/PACF plotting
    plot_ACF(acf_ma2, N_lag, N_sample, "MA of ACF")
    plot_ACF(acf_ar2, N_lag, N_sample, "AR of ACF")
    plot_PACF(pacf_ma2, N_lag, N_sample, "MA of PACF")
    plot_PACF(pacf_ar2, N_lag, N_sample, "AR of PACF")
    plt.show()

    # ========== Problem 4 ========== #
    run_csv(CSV_lag=15, filename="market-price.csv")
    plt.show()


if __name__ == "__main__":
    main()
