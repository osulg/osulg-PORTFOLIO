import os, glob, math, shutil
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

# 추가 라이브러리
import random  # 난수 생성을 위한 라이브러리
from sklearn.metrics import confusion_matrix  # 혼동행렬 계산 함수

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Please change "skeleton.py" name to your initial of name for scoring 
Eg) If your name is 홍길동 
--> skeleton.py --> HKD_assign1.py
"""

# ===================== Config =====================
IMG_SIZE = (128, 128)
IMG_EXTS = (".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff")
OUTDIR = "outputs"
# your personal config here

STUDENT_ID = "2023202031"  # 학생 학번
ASSIGNMENT_NAME = "Assign1_01"  # 과제 이름

SAVE_DIR = os.path.join(OUTDIR)  # 최종 저장 경로
os.makedirs(SAVE_DIR, exist_ok=True)  # SAVE_DIR 폴더가 없으면 생성

# 저장 경로 확인 로그
print(f"[Config] Save directory set to: {SAVE_DIR}")
# end


# ===================== I/O (제공) =====================
def list_images(root):
    subs = sorted([d for d in glob.glob(os.path.join(root, "*")) if os.path.isdir(d)])
    classes = [os.path.basename(d) for d in subs]
    paths, labels = [], []
    for ci, c in enumerate(classes):
        for f in sorted(glob.glob(os.path.join(root, c, "*"))):
            if os.path.splitext(f)[1].lower() in IMG_EXTS:
                paths.append(f)
                labels.append(ci)
    if not paths:
        raise RuntimeError(f"No images under {root}")
    paths_arr = np.array(labels, int)
    classes_list = classes
    return paths, paths_arr, classes_list


def build_X(paths, size=IMG_SIZE):
    X = []
    for p in paths:
        im = Image.open(p).convert("L").resize(size, Image.BILINEAR)
        X.append(np.asarray(im, dtype=np.float32).flatten())
    X_mat = np.stack(X, 0)
    return X_mat


def imsave(img2d, path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    plt.figure(figsize=(6, 6))
    plt.imshow(img2d, cmap="gray")
    plt.axis("off")
    plt.tight_layout(pad=0)
    plt.savefig(path, dpi=200)
    plt.close()


def grid(vectors, hw, ncols=4, norm=True):
    H, W = hw
    N = vectors.shape[0]
    R = math.ceil(N / ncols)
    G = np.zeros((R * H, ncols * W), np.float32)
    for i in range(N):
        r = i // ncols
        c = i % ncols
        im = vectors[i].reshape(H, W)
        if norm:
            mn, mx = im.min(), im.max()
            if mx > mn:
                im = (im - mn) / (mx - mn) * 255.0
        G[r * H : (r + 1) * H, c * W : (c + 1) * W] = im
    return G


# ===================== PCA (구현 영역) =====================
def compute_A_adj(X):
    """Zero-center the training data.
    Returns:
        A_adj: (N, D)
        mean_vec: (D,)
    """
    # parameter
    # N = 샘플 수,  D = 특징 수
    # 행렬 X의 크기를 읽어 N, D에 넣기
    N, D = X.shape

    # you should get this parameter correctly
    # zeros로 미리 만들어 둠
    A_adj = np.zeros((N, D), dtype=np.float32)  # 중심화 결과를 담을 배열 미리 생성
    mean_vec = np.zeros((D,), dtype=np.float32)  # 각 열(특징)의 평균을 저장할 벡터 생성

    # your code here

    # 1) train 데이터의 평균 구하기
    # 열 방향 평균 구하기
    # axis=0 -> 열 기준 평균 (D,)
    # float64로 누적/나눗셈을 수행 -> 오차 감소
    m = X.mean(axis=0, dtype=np.float64)

    # 2) mean_vec에 평균 벡터 저장
    # 계산된 평균을 float32로 변환해 mean_vec에 복사
    mean_vec[:] = m.astype(np.float32, copy=False)

    # 3) zero-centering
    # 각 샘플에서 같은 mean_vec을 뺌
    # 열 평균이 0에 가깝게 됨 -> PCA에서 공분산의 중심이 원점으로 맞춰짐
    # (N,D) -> (D, ) -> 각 행에서 동일한 평균을 빼 중심화
    A_adj[:] = X - mean_vec

    # end

    # zero-centering 결과 및 열 평균 벡터 반환
    return A_adj, mean_vec


def compute_covariance(A_adj):
    """Compute a covariance-like symmetric matrix for EVD.
    Hints:
        - For images (D >> N), consider the small-matrix trick from lecture.
        - Ensure the result is explicitly **symmetric** (numerical symmetry).
    Returns:
        L: (N, N) or (D, D)
    """
    # parameter
    # 입력 데이터의 크기 확인
    N, D = A_adj.shape

    # you should get this parameter correctly
    # 결과를 담을 행렬 L 초기화
    # NxN 크기의 행렬을 0으로 채움
    L = np.zeros((N, N), dtype=np.float64)

    # your code here

    # 1) 계산은 수치의 안정을 위해 float64로 형변환
    A = A_adj.astype(np.float64, copy=False)

    # 2) small-matrix trick
    # (NxD) @ (DxN) => (NxN) 행렬 생성
    # 분모는 공분산 정의에 따라 (N-1)로 설정
    demon = max(N - 1, 1)  # N-1이 0인 경우 방지
    temp_L = (A @ A.T) / demon  # 행렬 곱 후 평균을 나눔 -> 공분산

    # 3) 대칭화
    L[:] = (temp_L + temp_L.T) / 2.0

    # end
    return L  # 결과 반환


def eigen_decomposition(L, A_adj):
    """Eigen-decompose L and recover principal axes in data space.
    Returns:
        eigenvalues: (M,) sorted descending, nonnegative
        FeatureVector: (D, M) column-orthonormal axes (largest first)
    Notes:
        - Sort by eigenvalue (desc), drop tiny parts, normalize columns.
        - Use A_adj if you use the small-matrix approach.
        - Optional sanity check (commented):
            # q = min(FeatureVector.shape[1], 8)
            # I = FeatureVector[:, :q].T @ FeatureVector[:, :q]
            # assert np.allclose(I, np.eye(q), atol=1e-5)
    """
    # parameter
    # A_adj의 크기 확인
    N, D = A_adj.shape

    # you should get this parameter correctly
    # 결과 저장용 변수 초기화
    eigenvalues = np.zeros((min(L.shape),), dtype=np.float64)
    FeatureVector = np.zeros((D, eigenvalues.shape[0]), dtype=np.float64)

    # your code here

    # 1) L 행렬에 대해 고유값 분해
    # w : 오름차순 고유값
    # v : 열마다 L의 고유벡터
    w, v = np.linalg.eigh(L)

    # 2) 원본 데이터 공간의 주성분 축 (FeatureVector)을 복원
    u = A_adj.T @ v

    # 3) 고유값을 내림차순으로 정렬하기 위한 인덱스를 구함
    sorted_index = np.argsort(w)[::-1]

    # 4) 위 인덱스를 이용해 고유값, FeatureVector를 내림차순으로 정렬
    sorted_w = w[sorted_index]
    sorted_u = u[:, sorted_index]

    # 5) FeatureVector의 각 열(주성분 축)을 정규화 => 단위 벡터로(길이1)
    norms = np.linalg.norm(sorted_u, axis=0)
    # 0으로 나누는 것을 방지하기 위해 아주 작은 값을 더함
    normalized_u = sorted_u / (norms + 1e-12)

    # 6) 최종 결과를 미리 할당된 변수에 저장
    eigenvalues[:] = sorted_w
    FeatureVector = normalized_u

    # end
    return eigenvalues, FeatureVector  # 반환


def select_components(FeatureVector, kmax):
    """Pick the first kmax principal axes (largest first)."""
    # parameter
    # D = 특징 수(픽셀 수), M = 전체 주성분 개수
    D, M = FeatureVector.shape
    kmax = min(kmax, M)  # 주성분 개수가 전체 개수를 초과하지 않도록 설정

    # you should get this parameter correctly
    # 선택된 주성분 축을 저장할 배열 초기화 (D x kmax)
    FeatureVector_k = np.zeros((D, kmax), dtype=FeatureVector.dtype)

    # your code here
    # 1) 주성분 벡터 행렬에서 상위 kmax개의 열(주성분)을 선태
    if kmax > 0:
        # 이미 내림차순 정렬된 FeatureVector에서 kmax만큼 복사
        FeatureVector_k[:] = FeatureVector[:, :kmax]

    # end

    return FeatureVector_k  # 선택된 부분 반환


def transform_scores(A_adj, FeatureVector_k):
    """Project zero-centered data to scores A = A_adj @ V_k."""
    # parameter
    # N = 샘플 수, D = 특징 수
    N, D = A_adj.shape
    # D2 = 선택된 축의 차원, K = 축 개수 K
    D2, K = FeatureVector_k.shape

    # you should get this parameter correctly
    # 투영 결과 저장 버퍼
    A_scores = np.zeros((N, K), dtype=np.float64)

    # your code here

    # D == D2인지 확인
    assert D == D2, f"Shape mismatch: A_adj({D}) and FeatureVector_k({D2})"

    # 1) zero-centering된 데이터 A_adj를 선택된 주성분 축(FeatureVector_k)에 투영
    # 저차원 표현인 점수(A_scores) 계산
    A = A_adj.astype(np.float64, copy=False)
    Vk = FeatureVector_k.astype(np.float64, copy=False)
    A_scores[:] = A @ Vk  # 투영 점수 계산
    # end

    return A_scores  # 점수 행렬 반환 (NxK)


def reconstruct_images(A_scores, FeatureVector_k, mean_vec, k):
    """Reconstruct with first k scores/axes, then add mean."""
    # parameter
    # N = 샘플 수, kmax = 점수(축) 최대 개수
    N, Kmax = A_scores.shape
    # D = 특징 수, kmax2 = 최대 주성분 개수
    D, Kmax2 = FeatureVector_k.shape
    # k가 실제 존재하는 축 개수를 조정
    k = min(k, Kmax, Kmax2)

    # you should get this parameter correctly
    # 복원 결과를 저장할 배열 초기화
    X_hat = np.zeros((N, D), dtype=np.float64)

    # your code here

    # 1) 상위 k개의 점수와 주성분 축을 선택
    # scores_k : (N,k), vector_k : (D,k)
    # 작은 k일수록 복원 품질 ↓
    scores_k = A_scores[:, :k].astype(np.float64, copy=False)
    vectors_k = FeatureVector_k[:, :k].astype(np.float64, copy=False)

    # 2) 선택된 점수와 주성분 축의 전치를 곱하여
    # zero-centering된 데이터를 근사적으로 복원
    reconstructed_A_adj = scores_k @ vectors_k.T  # (N,D)

    # 3) 복원된 데이터에 평균 벡터(mean_vec)를 더해 최종 이미지 복원
    # 평균 벡터를 더해 원래 스케일로 복귀
    X_hat[:] = reconstructed_A_adj + mean_vec

    # end

    return X_hat  # 복원 결과 반환


# ===================== Classification & Plots (보고서를 위한 자율 공간) =====================
def knn_predict(trX, trY, teX, k=3):
    """(Optional) Students may replace this with their own classifier.
    Return: predicted labels for teX of shape (N_te,)."""
    # TODO (optional): implement k-NN or another simple classifier
    pred = np.zeros((teX.shape[0],), dtype=int)

    # 1) 각 test 샘플별로 train과 거리 계산
    for i, x in enumerate(teX):
        # 모든 train 샘플과의 유클리드 거리 계산
        distance = np.linalg.norm(trX - x, axis=1)

        # 2) 가까운 k개의 train 인덱스 선택
        # 거리가 작은 순으로 정렬하여 상위 k개만 추출
        nn_idx = np.argsort(distance)[:k]

        # 3) 이웃 라벨 가져오기 -> (k,)
        nn_labels = trY[nn_idx]

        # 4) 최종 예측 라벨 결정
        # 등장 횟수(bincount)가 가장 큰 라벨 선택
        pred[i] = np.bincount(nn_labels).argmax()

    return pred


def plot_cm(cm, names, path, title):
    """(Optional) Students plot confusion matrix in their style/colors."""
    # TODO (optional): implement a clean CM plot

    # 1) 저장할 폴더 생성 (/outputs/cm)
    base = os.path.basename(path)
    save_dir = os.path.join(OUTDIR, "cm")
    os.makedirs(save_dir, exist_ok=True)
    save_path = os.path.join(save_dir, base)

    # 2) 그래프 크기와 색상 설정
    plt.figure(figsize=(4, 4))
    plt.imshow(cm, cmap="Blues")  # 파란색 계열로 표현(정확도가 높을수록 진한 색)

    # 3) 제목/축 라벨
    plt.title(title, fontsize=10)
    plt.xlabel("knn-Predicted Label", fontsize=9)
    plt.ylabel("True Label", fontsize=9)

    # 4) 축 눈금을 설정 + 클래스명 표시
    plt.xticks(np.arange(len(names)), names, rotation=45, fontsize=8)
    plt.yticks(np.arange(len(names)), names, fontsize=8)

    # 5) 각 셀에 숫자(빈도) 표시
    for i in range(cm.shape[0]):
        for j in range(cm.shape[1]):
            # 셀 배경이 진할수록 숫자를 흰색으로 반전
            color = "white" if cm[i, j] > cm.max() / 2 else "black"
            plt.text(j, i, str(cm[i, j]), ha="center", va="center", color=color)

    # 6) 컬러바 추가 (비율 확인용)
    plt.colorbar()

    # 7) 이미지 저장 (200dpi 고해상도)
    plt.tight_layout()  # 여백 맞추기
    plt.savefig(save_path, dpi=200)
    plt.close()


def pc_scatter(A_scores_test, y_true, y_pred, names, i, j, path):
    """(Optional) Students plot PC1–PC2 true vs pred, or other 2D projections."""
    # TODO (optional): implement side-by-side scatter

    # 1) 저장 폴더 생성 alc 저장 경로 구성 (outputs/scatter)
    base = os.path.basename(path)
    save_dir = os.path.join(OUTDIR, "scatter")
    os.makedirs(save_dir, exist_ok=True)
    save_path = os.path.join(save_dir, base)

    # 점수의 사용 가능한 주성분 수
    num_pc = A_scores_test.shape[1]

    # 2) 캔버스 생성 (figure)
    # 왼 : 실제 라벨
    # 오 : 예측 라벨
    plt.figure(figsize=(8, 4))

    # 3) 2D 산점도 또는 1D 산점도 자동 전환
    if num_pc >= 2:
        # left : True Labels (2D)
        plt.subplot(1, 2, 1)  # 왼쪽 plot
        plt.scatter(
            A_scores_test[:, i],
            A_scores_test[:, j],
            c=y_true,
            cmap="viridis",
            alpha=0.6,
            edgecolors="k",
        )  # 실제 라벨에 대해(y_true)
        plt.title("True Labels (2D)")  # 제목
        plt.xlabel(f"PC{i+1}")  # x축 라벨
        plt.ylabel(f"PC{j+1}")  # y축 라벨
        plt.grid(True)

        # right : Predicted Labels (2D)
        plt.subplot(1, 2, 2)  # 오른쪽 plot
        plt.scatter(
            A_scores_test[:, i],
            A_scores_test[:, j],
            c=y_pred,
            cmap="viridis",
            alpha=0.6,
            edgecolors="k",
        )  # 예측 라벨에 대해(y_pred)
        plt.title("Predicted Labels (2D)")  # 제목
        plt.xlabel(f"PC{i+1}")  # x축 라벨
        plt.ylabel(f"PC{j+1}")  # y축 라벨
        plt.grid(True)

    else:
        # left : True Labels (1D)
        plt.subplot(1, 2, 1)  # 왼쪽 plot
        plt.scatter(
            np.arange(len(A_scores_test)),
            A_scores_test[:, 0],
            c=y_true,
            cmap="viridis",
            alpha=0.6,
            edgecolors="k",
        )  # x축 : 샘플 인덱스, y축 : 첫번쨰 PC (y_true)
        plt.title("True Labels (1D)")  # 제목
        plt.xlabel("Sample index")  # x축
        plt.ylabel("PC1 score")  # y축
        plt.grid(True)

        # right : Predicted Labels (1D)
        plt.subplot(1, 2, 2)  # 오른쪽 plot
        plt.scatter(
            np.arange(len(A_scores_test)),
            A_scores_test[:, 0],
            c=y_pred,
            cmap="viridis",
            alpha=0.6,
            edgecolors="k",
        )  # x축 : 샘플 인덱스, y축 : 첫번쨰 PC (y_pred)
        plt.title("Predicted Labels (1D)")  # 제목
        plt.xlabel("Sample index")  # x축
        plt.ylabel("PC1 score")  # y축
        plt.grid(True)

    # 4) 파일 저장 (200dpi 고해상도)
    plt.tight_layout()  # 여백 맞추기
    plt.savefig(save_path, dpi=200)
    plt.close()


def recon_panels_for_classes(
    Xte,
    A_scores_test,
    FeatureVector_k,
    mean_vec,
    te_paths,
    te_labels,
    class_names,
    outdir,
    ks=(1, 2, 4, 20),
    n_per_class=5,
):
    """(Optional) Students create reconstruction panels for report figures."""
    # TODO (optional): replicate reconstruction panel figure (orig + k in ks)

    # 1) 출력 폴더 준비
    # outdir : restruction 결과를 저장할 위치 (/outputs/recon)
    os.makedirs(outdir, exist_ok=True)

    # 2) 이미지 해상도 정보 (128x128)
    # 복원 후 reshape하는 경우 사용
    H, W = IMG_SIZE  # 128x128

    # 3) 클래스 단위로 반복
    for class_i, cName in enumerate(class_names):
        # 3-1) 현재 클래스에 해당하는 test 이미지 인덱스 찾기
        idx_list = np.where(te_labels == class_i)[0]

        # 클래스에 데이터가 존재하지 않는 경우 경고 문자
        if len(idx_list) == 0:
            print(f"[Warning] {cName} class no data")
            continue

        # 3-2) 클래스별 하위 폴더 생성
        class_dir = os.path.join(outdir, cName)
        os.makedirs(class_dir, exist_ok=True)

        # 3-3) 무작위로 n_per_class장 선택 (기본 5장)
        chosen_idx = random.sample(list(idx_list), min(n_per_class, len(idx_list)))

        # 3-4) 선택한 이미지 가져오기 및 zero-centering
        X_sel = Xte[chosen_idx]  # 선택된 원본 test 이미지 (NxD)
        A_adj_sel = X_sel - mean_vec  # 평균을 빼서 중심황

        # 4) k 값 변화에 따라 복원 반복
        for k in ks:
            # 4-1) 상위 k개 주성분 축 선택
            V_k = select_components(FeatureVector_k, k)

            # 4-2) 선택된 이미지들을 PCA 공간으로 투영
            A_scores_test = transform_scores(A_adj_sel, V_k)

            # 4-3) 투영 점수 + 축 + 평균으로 복원
            X_hat = reconstruct_images(A_scores_test, V_k, mean_vec, k)

            # 5) 복원 이미지 저장
            for i, idx in enumerate(chosen_idx):
                orig_path = te_paths[idx]  # 원본 파일 경로
                orig_name = os.path.basename(orig_path)  # ex) 125.jpeg
                save_path = os.path.join(class_dir, f"{cName}_{orig_name}_k{k}.png")
                img = X_hat[i].reshape(H, W)  # 128x128 복원
                imsave(img, save_path)  # PNG 저장

        # 복원 완료에 대한 로그 출력
        print(f"[{cName}] Complete restruction completed ->  ({class_dir})")


# ===================== Orchestration (main) =====================
def main():
    # 1) Data
    tr_paths, tr_y, classes = list_images("train")
    te_paths, te_y, _ = list_images("test")
    X_tr = build_X(tr_paths, IMG_SIZE)
    X_te = build_X(te_paths, IMG_SIZE)

    # 2) A_adj (train zero-centered); test also centered with train mean
    A_adj_tr, mean_vec = compute_A_adj(X_tr)
    A_adj_te = X_te - mean_vec

    # 3) Covariance-like matrix for EVD
    L = compute_covariance(A_adj_tr)

    # 4) EVD -> eigenvalues, FeatureVector (principal axes)
    eigenvalues, FeatureVector = eigen_decomposition(L, A_adj_tr)

    # 5) For report

    # your code here

    # ========== Problem1 ========== #
    print("# ========== Problem1 ========== #")

    # 1) compute_A_adj
    A_adj, mean_vec = compute_A_adj(X_tr)  # Train 데이터 X_tr을 이용해 중심화 수행
    print("--- compute_A_adj ---\n")
    print("Mean shape:", mean_vec.shape)  # 평균 벡터 출력
    print("A_adj shape:", A_adj.shape)  # 중심화된 데이터 출력
    print("Mean of A_adj:", np.mean(A_adj))  # 중심화된 데이터의 평균값 확인
    print("\n")

    # 2) compute_covariance
    L = compute_covariance(A_adj_tr)  # zero-centering된 훈련 데이터로 공분산 계산
    print("--- compute_covariance ---\n")
    print("L shape:", L.shape)  # L의 크기 확인
    print("L symmetric?", np.allclose(L, L.T))  # 대칭성 검사
    print("\n")

    # 3) eigen_decomposition()
    eigenvalues, FeatureVector = eigen_decomposition(
        L, A_adj_tr
    )  # small-matrix L과 중심화된 A_adj_tr로 EVD 수행
    print("--- eigen_decomposition ---\n")
    print("Eigenvalues shape:", eigenvalues.shape)  # 고유값 벡터 크기 확인
    print("FeatureVector shape:", FeatureVector.shape)  # FeatureVector의 크기 확인
    print(
        "Orthogonality check:",
        np.allclose(
            FeatureVector[:, :5].T @ FeatureVector[:, :5], np.eye(5), atol=1e-5
        ),
    )  # 직교성 확인 (주성분 5개로 확인)
    print("\n")

    # 4) select_components()
    FeatureVector_k = select_components(
        FeatureVector, 20
    )  # 전체 500개의 주성분 중 상위 20개만 선택
    print("--- select_components ---\n")
    print("Shape:", FeatureVector_k.shape)  # 선택된 FeatureVector_k를 확인
    # kmax가 20
    print("\n")

    # 5) transform_scores
    A_scores = transform_scores(
        A_adj_tr, FeatureVector_k
    )  # Train 데이터(zero-centering)와 상위 20개의 축을 이용해 투영
    print("--- transform_scores ---\n")
    print("Shape:", A_scores.shape)  # 투영 결과 확인
    print(
        "Mean of A_scores:", np.mean(A_scores, axis=0)[:5]
    )  # 각 주성분 축 방향의 평균값 확인
    print("\n")

    # 6) reconstruct_images()
    X_hat = reconstruct_images(
        A_scores, FeatureVector_k, mean_vec, k=20
    )  # 상위 20개의 주성분으로 복원
    print("--- reconstruct_images ---\n")
    print("Shape:", X_hat.shape)  # 복원된 결과 확인
    print("\n")

    print("[INFO] Problem 1 completed.\n")

    # ========== Problem2 ========== #

    print("# ========== Problem2 ========== #\n")

    # 1) 주성분 개수 리스트
    # + 클래스 당 선택 이미지 수
    ks = (1, 5, 10, 20, 30, 50, 75, 100)
    n_per_class = 5

    # 2) Test 데이터 Zero-centering
    A_adj_te = X_te - mean_vec

    # 3) 복원 결과 저장
    # 결과적으로 /outputs/recon/각 클래스명{Bears/Pandas}에 저장
    recon_outdir = os.path.join(OUTDIR, "recon")

    # 기존 결과가 존재한다면 삭제 후 새로 생성
    if os.path.exists(recon_outdir):
        print(
            f"[INFO] Previous reconstruction results found at {recon_outdir}. Removing old files."
        )
        shutil.rmtree(recon_outdir)
    os.makedirs(recon_outdir, exist_ok=True)

    # 이미지 복원
    recon_panels_for_classes(
        X_te,
        None,
        FeatureVector,
        mean_vec,
        te_paths,
        te_y,
        classes,
        recon_outdir,
        ks=ks,
        n_per_class=n_per_class,
    )

    print("[INFO] Reconstruction for Problem 2 completed.\n")

    # ========== Problem3 ========== #
    print("# ========== Problem3 ========== #\n")

    # 주성분 개수 리스트
    ks = [1, 2, 10, 30, 75, 100]
    accuracies = []  # 정확도 저장용 리스트

    for k in ks:
        print(f"[INFO] Running PCA+KNN with k={k}")

        # 1) 상위 k개 주성분 선택
        V_k = select_components(FeatureVector, k)

        # 2) train/test 데이터 투영 (PCA)
        A_scores_tr = transform_scores(A_adj_tr, V_k)
        A_scores_te = transform_scores(A_adj_te, V_k)

        # 3) KNN 분류 (여기서 k는 KNN의 최근접 이웃 수도 동일)
        pred = knn_predict(A_scores_tr, tr_y, A_scores_te, k=3)

        # 4) 정확도 계산
        accuracy = np.mean(pred == te_y)
        accuracies.append(accuracy)
        print(f"Accuracy (k={k}): {accuracy:.4f}")  # 근접도 출력

        # 5) 혼동 행렬 시각화
        cm = confusion_matrix(te_y, pred)
        plot_cm(
            cm, classes, f"{OUTDIR}/cm_k{k}.png", f"PCA-KNN Confusion Matrix (k={k})"
        )

        # 6) 주성분 시각화
        # 주성분이 2개 이상일 때는 2D scatter, 1개일 때는 1D scatter 자동 처리
        pc_scatter(A_scores_te, te_y, pred, classes, 0, 1, f"{OUTDIR}/scatter_k{k}.png")

    # 7) 정확도 경로 설정 (outputs/accuracy)
    acc_dir = os.path.join(OUTDIR, "accuracy")
    os.makedirs(acc_dir, exist_ok=True)
    acc_path = os.path.join(acc_dir, "accuracy_curve.png")

    # 8) 정확도 변화 그래프
    plt.figure(figsize=(6, 4))
    plt.plot(ks, accuracies, marker="o", color="Blue")
    plt.title("PCA + KNN Classification Accuracy")
    plt.xlabel("Number of Principal Components (k)")  # x축
    plt.ylabel("Accuracy")  # y축
    plt.grid(True)

    # 8) 그래프 저장 (200dpi 고해상도)
    plt.tight_layout()  # 여백 맞추기
    plt.savefig(acc_path, dpi=200)
    plt.close()

    print("[INFO] Problem 3 completed.")

    # end


if __name__ == "__main__":
    main()
